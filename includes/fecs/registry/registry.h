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
     * @brief Acts as a main interface for the ECS, manages a environment for entities, and component manipulation.
     *
     * The registry handes entity creation and destruction, component attachment/detachment,
     * exposes access to Component Pool and Views
     * NOTE: All registries share the same component pool. This is an intended function.
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

        // ====================================================================
        //                         ENTITY MANAGEMENT
        // ====================================================================

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

        // ====================================================================
        //                         COMPONENT MANAGEMENT
        // ====================================================================

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
         * @brief Emplaces a component to an entity
         * @tparam T The component type.
         * @param e The target entity.
         * @param args The args to attach.
         */
        template <typename T, typename... Args>
        T& Emplace(Entity e, Args&&... args)
        {
            Container::SparseSet<T>& set = ComponentManager::GetPool<T>(&m_EntityManager);
            T& component = set.Emplace(e, std::forward<Args>(args)...);

            // Update the component's version to signal
            ComponentManager::GetVersion<T>()++;

            return component;
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
         * @brief Safely retrieves a component pointer attached to an entity.
         * @tparam T The component type.
         * @param e The target entity.
         * @return Pointer to the component, or nullptr if it doesn't exist.
         */
        template <typename T>
        T* TryGet(Entity e)
        {
            if (!Has<T>(e))
            {
                return nullptr;
            }
            // We know it exists, so this Get<T> call is safe
            return &Get<T>(e);
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

        // ====================================================================
        //                       SINGLETON & CONTEXT
        // ====================================================================

        /**
         * @brief Retrieves the first instance of a specified component type.
         *
         * @tparam C The component type to retrieve.
         * @return Pointer to the component instance of type C, or nullptr if not found.
         */
        template <typename C>
        C* Ctx()

        {
            C* result = nullptr;
            View<C>().Each([&](Entity, C& c)
            {
                result = &c;
                return false;
            });
            return result;
        }

        /**
         * @brief Retrieves the first instance of a component, creating it if none exists.
         *
         * Useful for singleton/context components (e.g., Time, PhysicsSettings).
         * If no entity has component C, a new entity is created and C is
         * emplaced on it.
         *
         * @tparam C The component type.
         * @tparam Args... Constructor argument types if creating.
         * @return Reference to the component instance.
         */
        template <typename C, typename... Args>
        C& GetOrCreateSingleton(Args&&... args)
        {
            C* component = Ctx<C>();
            if (component)
            {
                return *component;
            }

            // Not found, create one
            Entity newSingletonEntity = CreateEntity();
            return Emplace<C>(newSingletonEntity, std::forward<Args>(args)...);
        }

        /**
         * @brief Retrieves the first entity containing the specified component type.
         *
         * Iterates over all entities that have a component of type C and returns
         * the ID of the first one found. If no such entity exists, returns
         * INVALID_ENTITY.
         *
         * @tparam C
         *    The component type to search for.
         * @return
         *    The first entity ID that has component C, or INVALID_ENTITY if none is found.
         */
        template <typename C>
        FECS::Entity EntityCtx()
        {
            FECS::Entity result = INVALID_ENTITY;
            View<C>().Each([&](FECS::Entity id, C&)
            {
                result = id;
                return false;
            });
            return result;
        }

        // ====================================================================
        //                         CONVENIENCE FUNCTIONS
        // ====================================================================

        /**
         * @brief Retrieve a component, or default-construct it if not already present.
         * @tparam C The component type (must be default-constructible).
         * @param id The entity identifier.
         * @return Reference to the component.
         */
        template <typename C>
        C& GetOrAttach(FECS::Entity id)
        {
            if (!Has<C>(id))
            {
                return Emplace<C>(id);
            }
            return Get<C>(id);
        }

        /**
         * @brief Retrieve a component, or emplace-construct it with args if not present.
         * @tparam C The component type.
         * @param id The entity identifier.
         * @param args... Arguments for C's constructor.
         * @return Reference to the component.
         */
        template <typename C, typename... Args>
        C& GetOrAttach(FECS::Entity id, Args&&... args)
        {
            if (!Has<C>(id))
            {
                return Emplace<C>(id, std::forward<Args>(args)...);
            }
            return Get<C>(id);
        }

        // ====================================================================
        //                           ITERATION & VIEWS
        // ====================================================================

        /**
         * @brief Creates a view for the specified component types.
         * @tparam C... The component types to include in the view.
         * @return A View object for iterating over matching entities.
         */
        template <typename... C>
        FECS::View<C...>& View()
        {
            // Create and return the view created
            static FECS::View<C...> v = FECS::View<C...>(&m_EntityManager);
            return v;
        }

        // ====================================================================
        //                         MEMORY MANAGEMENT
        // ====================================================================

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

    private:
        Manager::EntityManager m_EntityManager; ///< The internal entity manager.
    };
}
