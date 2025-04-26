#include "fecs/core/core.hpp"
#include <fecs/core/entity/entity.hpp>
#include <fecs/manager/entity/entity_manager.hpp>

namespace FECS
{

    Entity::Entity(Registry& registry, Manager::EntityManager& entityManager)
        : m_RegistryReference(registry),
          m_EntityManagerReference(entityManager)
    {
    }

    std::uint32_t Entity::GetID() const
    {
        return m_ID;
    }

    void Entity::SetID(std::uint32_t id)
    {
        m_ID = id;
    }

    bool Entity::IsValid() const
    {
        return m_ID != INVALID_ENTITY_INDEX;
    }

    void Entity::Destroy()
    {
        m_EntityManagerReference.DestroyEntity(*this);
    }

    // Operators
    Entity::operator std::uint32_t() const
    {
        return GetID();
    }
}
