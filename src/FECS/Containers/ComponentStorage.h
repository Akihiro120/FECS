#pragma once
#include <cstdint>
#include <vector>
#include "FECS/Core/Types.h"
#include "FECS/Containers/ISparseSet.h"
#include "FECS/Containers/SparseSet.h"

class ComponentIndex
{
public:
    template <typename T>
    static auto GetIndex() -> std::uint32_t
    {
        static const std::uint32_t idx = m_Index++;
        return idx;
    }

private:
    static inline std::uint32_t m_Index = 0;
};

namespace FECS::Container
{
    class ComponentStorage
    {
    public:
        ComponentStorage() = default;
        ~ComponentStorage()
        {
            // clear all heap allocated memory
            for (auto* components : m_Components)
            {
                if (components)
                {
                    delete components;
                }
            }
        }

        template <typename T>
        auto GetPool() -> Container::SparseSet<T>*
        {
            std::uint32_t idx = ::ComponentIndex::GetIndex<T>();
            if (idx >= m_Components.size())
            {
                m_Components.resize(idx + 1, nullptr);
            }

            if (!m_Components[idx])
            {
                m_Components[idx] = new Container::SparseSet<T>();
            }

            return static_cast<Container::SparseSet<T>*>(m_Components[idx]);
        }

        template <typename T>
        auto GetVersion() -> std::uint32_t&
        {
            std::uint32_t idx = ::ComponentIndex::GetIndex<T>();
            if (idx >= m_Versions.size())
            {
                m_Versions.resize(idx + 1, 0);
            }

            return m_Versions[idx];
        }

        auto Reserve(std::size_t numComponents) -> void
        {
            m_Components.reserve(numComponents);
        }

        auto DeleteEntity(Entity e) -> void
        {
            for (auto& comps : m_Components)
            {
                comps->Remove(e);
            }
        }

        auto ClearRegistry() -> void
        {
            for (auto& comps : m_Components)
            {
                comps->Clear();
            }

            m_Components.clear();
            m_Versions.clear();
        }

    private:
        std::vector<Container::ISparseSet*> m_Components;
        std::vector<std::uint32_t> m_Versions;
    };
}
