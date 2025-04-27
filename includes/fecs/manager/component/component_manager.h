#pragma once
#include <typeindex>
#include <memory>
#include <unordered_map>
#include <fecs/containers/sparse/sparse_set.h>

namespace FECS
{
    class Entity;

    namespace Manager
    {
        class QueryManager;
    }

    namespace Manager
    {

        class ComponentManager
        {
        public:
            ComponentManager(QueryManager& queryManager);
            ~ComponentManager() = default;

            template <typename Component>
            void AttachComponent(std::uint32_t id, const Component& component);

            template <typename Component>
            void DetachComponent(std::uint32_t id);

            template <typename Component>
            Component& Get(std::uint32_t id);

            template <typename Component>
            bool Has(std::uint32_t id);

            void RemoveEntity(std::uint32_t id);

        private:
            QueryManager& m_QueryManagerReference;
            std::unordered_map<std::type_index, std::unique_ptr<Container::ISparseSet>> m_ComponentPool;
        };
    }
}

#include "component_manager.inl"
