#include <fecs/manager/entity/entity_manager.hpp>
#include <fecs/core/entity/entity.hpp>
#include <fecs/registry/registry.hpp>

namespace FECS
{

    namespace Manager
    {

        /**
         * @brief Constructs an EntityManager with a reference to the given registry.
         *
         * Associates the entity manager with the provided FECS::Registry for managing entities.
         */
        EntityManager::EntityManager(FECS::Registry& registry)
            : m_RegistryReference(registry)
        {
        }

        /**
         * @brief Creates and returns a new entity with a unique ID.
         *
         * If there are previously destroyed entities, their IDs are reused; otherwise, a new unique ID is assigned.
         * @return FECS::Entity The newly created entity.
         */
        FECS::Entity EntityManager::CreateEntity()
        {
            FECS::Entity newEntity(m_RegistryReference, *this);

            // provide new id
            if (m_FreeID.size() != 0)
            {
                newEntity.SetID(m_FreeID[0]);
                m_FreeID.pop_back();

                return newEntity;
            }

            // give new id
            newEntity.SetID(++m_NextID);

            return newEntity;
        }

        /**
         * @brief Destroys an entity and recycles its ID for future use.
         *
         * Marks the given entity as destroyed by adding its ID to the pool of reusable IDs and invalidating the entity's ID.
         */
        void EntityManager::DestroyEntity(FECS::Entity& entity)
        {
            m_FreeID.push_back(entity.GetID());

            entity.SetID(std::numeric_limits<std::uint32_t>::max());
        }
    }
}
