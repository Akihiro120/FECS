#pragma once
#include "fecs/containers/sparse/sparse_set.hpp"
#include <fecs/manager/component/component_manager.h>
#include <typeindex>
#include <stdexcept>

namespace FECS
{

    namespace Manager
    {
        using namespace FECS::Container;

        template <typename Component>
        inline void ComponentManager::AttachComponent(std::uint32_t id, const Component& component)
        {
            auto& typeID = typeid(Component);
            auto it = m_ComponentPool.find(typeID);
            if (it == m_ComponentPool.end())
            {
                // attach
                m_ComponentPool[typeID] = std::make_unique<SparseSet<Component>>();
            }

            // replace
            auto& storage = static_cast<SparseSet<Component>&>(*m_ComponentPool[typeID]);
            storage.Insert(id, component);
        }

        template <typename Component>
        inline void ComponentManager::DetachComponent(std::uint32_t id)
        {
            auto& typeID = typeid(Component);
            auto it = m_ComponentPool.find(typeID);
            if (it != m_ComponentPool.end())
            {
                // detach
                auto& storage = static_cast<SparseSet<Component>&>(*m_ComponentPool[typeID]);
                storage.Remove(id);
            }
            else
            {
                throw std::runtime_error("Component Manager: Component not Found");
            }
        }

        template <typename Component>
        inline Component& ComponentManager::Get(std::uint32_t id)
        {
            auto& typeID = typeid(Component);
            auto it = m_ComponentPool.find(typeID);
            if (it != m_ComponentPool.end())
            {
                // detach
                auto& storage = static_cast<SparseSet<Component>&>(*m_ComponentPool[typeID]);
                return *storage.Get(id);
            }

            throw std::runtime_error("Component Manager: Component not Found");
        }
    }
}
