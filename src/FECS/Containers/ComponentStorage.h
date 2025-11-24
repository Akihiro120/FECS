#pragma once
#include <cstdint>
#include <vector>
#include "FECS/Core/Types.h"
#include "FECS/Containers/ISparseSet.h"
#include "FECS/Containers/SparseSet.h"

namespace
{
    class ComponentIndex
    {
    public:
        template <typename T>
        static std::uint32_t GetIndex()
        {
            static const std::uint32_t idx = m_Index++;
            return idx;
        }

    private:
        static inline std::uint32_t m_Index = 0;
    };
}

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
                delete components;
            }
        }

        template <typename T>
        Container::SparseSet<T>* GetPool()
        {
            std::uint32_t idx = ::ComponentIndex::GetIndex<T>();
            if (m_Components.size() <= idx)
            {
                Container::SparseSet<T>* newStorage = new Container::SparseSet<T>();
                m_Components.push_back(newStorage);
            }

            return reinterpret_cast<Container::SparseSet<T>*>(m_Components[idx]);
        }

        template <typename T>
        std::uint32_t& GetVersion()
        {
            std::uint32_t idx = ::ComponentIndex::GetIndex<T>();
            if (m_Versions.size() <= idx)
            {
                m_Versions.push_back(0);
            }

            return m_Versions[idx];
        }

        void Reserve(std::size_t numComponents)
        {
            m_Components.reserve(numComponents);
        }

        void DeleteEntity(Entity e)
        {
            for (auto& comps : m_Components)
            {
                comps->Remove(e);
            }
        }

        void ClearRegistry()
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
