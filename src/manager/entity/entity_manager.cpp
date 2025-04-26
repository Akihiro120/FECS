#include <fecs/manager/entity/entity_manager.hpp>
#include <fecs/core/entity/entity.hpp>
#include <fecs/registry/registry.hpp>

namespace FECS
{

    namespace Manager
    {

        EntityManager::EntityManager(FECS::Registry& registry)
            : m_RegistryReference(registry)
        {
        }

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

        void EntityManager::DestroyEntity(FECS::Entity& entity)
        {
            m_FreeID.push_back(entity.GetID());

            entity.SetID(std::numeric_limits<std::uint32_t>::max());
        }
    }
}
