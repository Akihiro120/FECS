#include <fecs/registry/registry.h>
#include <fecs/core/entity/entity.h>

namespace FECS
{

    Registry::Registry()
        : m_EntityManager(*this, m_QueryManager, m_ComponentManager),
          m_ComponentManager(m_QueryManager),
          m_QueryManager(m_ComponentManager)
    {
    }

    Registry::~Registry()
    {
    }

    Entity Registry::CreateEntity()
    {
        Entity newEntity = m_EntityManager.CreateEntity();
        return newEntity;
    }

    void Registry::DestroyEntity(Entity& entity)
    {
        m_EntityManager.DestroyEntity(entity);
    }
}
