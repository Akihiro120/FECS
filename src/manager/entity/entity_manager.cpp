#include <fecs/manager/entity/entity_manager.h>
#include <fecs/core/entity/entity.h>
#include <fecs/manager/component/component_manager.h>
#include <fecs/registry/registry.h>

namespace FECS
{

    namespace Manager
    {

        EntityManager::EntityManager(FECS::Registry& registry, QueryManager& queryManager, ComponentManager& componentManager)
            : m_RegistryReference(registry),
              m_QueryManagerReference(queryManager),
              m_ComponentManagerReference(componentManager)
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
            }
            else
            {
                // give new id
                newEntity.SetID(++m_NextID);
            }

            // give a signature
            m_QueryManagerReference.CreateEntitySignature(newEntity.GetID());

            return newEntity;
        }

        void EntityManager::DestroyEntity(FECS::Entity& entity)
        {
            m_QueryManagerReference.RemoveEntitySignature(entity.GetID());
            m_ComponentManagerReference.RemoveEntity(entity.GetID());

            m_FreeID.push_back(entity.GetID());
            entity.SetID(INVALID_ENTITY_INDEX);
        }
    }
}
