#include <fecs/registry/registry.hpp>
#include <fecs/core/entity/entity.hpp>

namespace FECS
{

    Registry::Registry()
        : m_EntityManager(*this)
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
