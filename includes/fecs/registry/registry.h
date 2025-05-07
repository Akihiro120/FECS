/**
 * @file registry.h
 * @brief Provides the central interface for managing entities and components
 */

#pragma once
#include <fecs/containers/sparse_set.h>
#include <fecs/manager/component_manager.h>
#include <fecs/manager/entity_manager.h>
#include <fecs/manager/view_manager.h>

namespace FECS
{
    using namespace FECS::Manager;

    /**
     * @class Registry
     * @brief Acts as a main interface for the ECS, manages a `**Local**` environment for entities, and component manipulation
     *
     * The registry handes entity creation and destruction, component attachment/detachment,
     * exposes access to Component Pool and Views
     */
    class Registry
    {
    public:
        /**
         * @brief Constructs a new Registry Instance.
         */
        Registry()
        {
        }

        /**
         * @brief Creates a new Entity
         * @return The newly created Entity ID.
         */
        Entity CreateEntity()
        {
            // Create an entity
            return m_EntityManager.Create();
        }

        /**
         * @brief Destroys an Entity and Removes its components
         * @param id The ID of the entity to destroy.
         */
        void DestroyEntity(Entity id)
        {
            // Remove the entity from all components
            ComponentManager::DeleteEntity(id);

            // Signal change in version for all
            ComponentManager::GetVersion<GlobalComponent>()++;

            // Destroy the entity
            m_EntityManager.Destroy(id);
        }

        /**
         * @brief Checks whether the given entity is still alive.
         * @param id The ID of the entity to check.
         * @return True if the entity is alive, otherwise false.
         */
        bool IsEntityAlive(Entity id) const
        {
            return m_EntityManager.IsAlive(id);
        }

        /**
         * @brief Get the local entity manager
         * @return A reference to the entity manager
         */
        EntityManager& GetEntityManager()
        {
            return m_EntityManager;
        }

        /**
         * @brief Reserve a predicted amount of components
         * @param size The number of predicted components
         */
        void Reserve(std::size_t size)
        {
            // Reserve the number of components to be used
            ComponentManager::Reserve(size);
        }

        /**
         * @brief Registers a new Component Type and returns its Pool
         * @tparam T The component type.
         * @return Reference to the component's SparseSet pool.
         */
        template <typename T>
        Container::SparseSet<T>& RegisterComponent()
        {
            // Register the component
            return ComponentManager::GetPool<T>(&m_EntityManager);
        }

        /**
         * @brief Retrieves the pool for a given component type.
         * @tparam T The component type.
         * @return Reference to the component's Sparse Set pool.
         * @note This function may pay the allocation cost for the component pool
         * use `RegisterComponent<T>()` to avoid that cost.
         */
        template <typename T>
        Container::SparseSet<T>& GetPool()
        {
            // Get the component, and register it if it doesnt exist already
            return ComponentManager::GetPool<T>(&m_EntityManager);
        }

        /**
         * @brief Attachs a component to an entity
         * @tparam T The component type.
         * @param e The target entity.
         * @param component The component to attach.
         */
        template <typename T>
        void Attach(Entity e, const T& component)
        {
            // Get the component pool, and insert the component for that entity
            Container::SparseSet<T>& set = ComponentManager::GetPool<T>(&m_EntityManager);
            set.Insert(e, component);

            // Update the component's version to signal
            ComponentManager::GetVersion<T>()++;
        }

        /**
         * @brief Retrieves a component reference attached to an entity.
         * @tparam T The component type.
         * @param e The target entity.
         * @return Reference to the component.
         */
        template <typename T>
        T& Get(Entity e)
        {
            // Get the component pool, and return the component associated with the entity
            Container::SparseSet<T>& set = ComponentManager::GetPool<T>(&m_EntityManager);
            return set.Get(e);
        }

        /**
         * @brief Detaches a component from an entity.
         * @tparam T The component type.
         * @param e The target entity
         */
        template <typename T>
        void Detach(Entity e)
        {
            // Get the component pool, and remove the component associated with the entity
            Container::SparseSet<T>& set = ComponentManager::GetPool<T>(&m_EntityManager);
            set.Remove(e);

            // Update the component's version to signal
            ComponentManager::GetVersion<T>()++;
        }

        /**
         * @brief Checks if a given component type exists on an entity.
         * @tparam T The component type.
         * @param e The target entity.
         */
        template <typename T>
        bool Has(Entity e)
        {
            // Get the component pool, and check the component associated with the entity
            Container::SparseSet<T>& set = ComponentManager::GetPool<T>(&m_EntityManager);
            return set.Has(e);
        }

        /**
         * @brief Creates a view for the specified component types.
         * @tparam C... The component types to include in the view.
         * @return A View object for iterating over matching entities.
         */
        template <typename... C>
        FECS::View<C...> View()
        {
            // Create and return the view created
            return FECS::View<C...>(&m_EntityManager);
        }

    private:
        Manager::EntityManager m_EntityManager; ///< The internal entity manager.
    };
}
