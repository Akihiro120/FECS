#pragma once
#include "fecs/core/types.h"
#include "fecs/manager/entity_manager.h"

namespace FECS::Registries
{

    class EntityRegistry
    {
    public:
        EntityRegistry() = default;
        EntityRegistry(const EntityRegistry&) = delete;
        EntityRegistry& operator=(const EntityRegistry&) = delete;

        FECS::Entity Create()
        {
            return m_EntityManager.Create();
        }

        void Reserve(size_t capacity)
        {
            m_EntityManager.Reserve(capacity);
        }

        void Destroy(FECS::Entity entityID)
        {
            return m_EntityManager.Destroy(entityID);
        }

        bool IsAlive(FECS::Entity entityID)
        {
            return m_EntityManager.IsAlive(entityID);
        }

    private:
        FECS::Manager::EntityManager m_EntityManager;
    };
}
