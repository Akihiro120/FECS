#pragma once
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <any>

namespace FECS::Manager
{
    class ResourceManager
    {
    public:
        ResourceManager()
        {
        }

        template <typename T>
        auto Add(const T& resource) -> void
        {
            auto typeId = std::type_index(typeid(T));
            auto it = m_Lookup.find(typeId);

            if (it != m_Lookup.end())
            {
                m_Data[it->second] = resource;
                return;
            }

            m_Lookup[typeId] = m_Data.size();
            m_Data.emplace_back(resource);
        }

        template <typename T, typename... Args>
        auto Emplace(Args&&... args) -> void
        {
            auto typeId = std::type_index(typeid(T));
            auto it = m_Lookup.find(typeId);
            if (it != m_Lookup.end())
            {
                m_Data[it->second] = std::make_any<T>(std::forward<Args>(args)...);
                return;
            }

            m_Lookup[typeId] = m_Data.size();
            m_Data.emplace_back(std::make_any<T>(std::forward<Args>(args)...));
        }

        template <typename T>
        auto Has() const -> bool
        {
            return m_Lookup.find(std::type_index(typeid(T))) != m_Lookup.end();
        }

        template <typename T>
        auto Get() -> T&
        {
            auto typeId = std::type_index(typeid(T));
            auto it = m_Lookup.find(typeId);

            assert(it != m_Lookup.end() && "ResourceManager - Resource does not exist.");

            return std::any_cast<T&>(m_Data[it->second]);
        }

    private:
        std::vector<std::any> m_Data;
        std::unordered_map<std::type_index, size_t> m_Lookup;
    };
}
