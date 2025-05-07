/**
 * @file component_manager.h
 * @brief Manages component pools and provides utility functions for component lifecycle operations.
 */

#pragma once
#include <fecs/core/types.h>
#include <fecs/containers/sparse_set.h>
#include <fecs/manager/entity_manager.h>

namespace FECS
{
    namespace Manager
    {
        /**
         * @brief Provides static access to component pools and handles entity cleanup
         *
         * The ComponentManager is a singleton-like utility with static methods and per-type
         * static storage (via Templates) to manage components, and versions efficiently.
         */
        class ComponentManager
        {
        private:
            /// Stores all component pools as ISparseSet pointers for type-erased operations.
            inline static std::vector<Container::ISparseSet*> m_RegisteredComponents;

            /// Private constructor to prevent instantiation.
            ComponentManager() = default;

        public:
            /**
             * @brief Get the component pool for a specific component type.
             *
             * If the pool doesnt exist yet, it's created and registered.
             *
             * @tparam T The component type.
             * @param manager Pointer to the EntityManager | `nullptr`.
             * @return Reference to the SparseSet for type T.
             *
             * @note This is a raw method to get the entity manager, its already handled via the Registry.
             */
            template <typename T>
            static Container::SparseSet<T>&
            GetPool(EntityManager* manager)
            {
                static Container::SparseSet<T> pool;

                // If the pool hasn't been initialized yet, set its EntityManager and register it.
                if (!pool.GetEntityManager())
                {
                    m_RegisteredComponents.push_back(&pool);
                    pool.SetEntityManager(manager);
                }
                return pool;
            }

            /**
             * @brief Returns a per-type version reference for tracking changes.
             *
             * Used to invalidate views and other systems when component data changes.
             *
             * @tparam T The component type.
             * @return Reference to a static version counter.
             */
            template <typename T>
            static std::uint32_t& GetVersion()
            {
                static std::uint32_t version;
                return version;
            }

            /**
             * @brief Reserves storage space for predicted number of component types.
             *
             * @param size Number of components to reserve for
             *
             * @note This is a raw method to reserve components, its already handled via the Registry.
             */
            static void Reserve(std::size_t size)
            {
                m_RegisteredComponents.reserve(size);
            }

            /**
             * @brief Removes an entity from all registered component pools.
             *
             * Used during entity destruction to clean up attached components.
             *
             * @param e The entity to remove.
             */
            static void DeleteEntity(Entity e)
            {
                for (auto& comps : m_RegisteredComponents)
                {
                    comps->Remove(e);
                }
            }

            /**
             * @brief Clears all component pools and resets internal state.
             *
             * Typically used in testing or to reset the ECS state.
             */
            static void ClearRegistry()
            {
                for (auto& comps : m_RegisteredComponents)
                {
                    comps->SetEntityManager(nullptr);
                    comps->Clear();
                }

                m_RegisteredComponents.clear();
            }
        };
    }
}
