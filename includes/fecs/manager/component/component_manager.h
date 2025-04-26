#pragma once
#include <typeindex>
#include <unordered_map>
#include <fecs/containers/sparse/sparse_set.hpp>

/**
 * @brief Manages components associated with entities.
 *
 * Provides methods to attach, detach, and retrieve components of various types for entities identified by their unique IDs.
 */
namespace FECS
{
    class Entity;

    namespace Manager
    {

        class ComponentManager
        {
        public:
            ComponentManager();
            ~ComponentManager() = default;

            template <typename Component>
            void AttachComponent(std::uint32_t id, const Component& component);

            template <typename Component>
            void DetachComponent(std::uint32_t id);

            template <typename Component>
            Component& Get(std::uint32_t id);

        private:
            std::unordered_map<std::type_index, std::unique_ptr<Container::ISparseSet>> m_ComponentPool;
        };
    }
}

#include "component_manager.inl"
