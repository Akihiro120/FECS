#include "fecs/core/core.hpp"
#include <fecs/core/entity/entity.hpp>
#include <fecs/manager/entity/entity_manager.hpp>

namespace FECS
{

    /**
     * @brief Constructs an Entity with references to a Registry and an EntityManager.
     *
     * Initializes the Entity to operate within the specified Registry and EntityManager context.
     */
    Entity::Entity(Registry& registry, Manager::EntityManager& entityManager)
        : m_RegistryReference(registry),
          m_EntityManagerReference(entityManager)
    {
    }

    /**
     * @brief Returns the unique identifier of the entity.
     *
     * @return The entity's 32-bit unsigned integer ID.
     */
    std::uint32_t Entity::GetID() const
    {
        return m_ID;
    }

    /**
     * @brief Sets the entity's identifier to the specified value.
     *
     * @param id The new entity ID.
     */
    void Entity::SetID(std::uint32_t id)
    {
        m_ID = id;
    }

    /**
     * @brief Checks whether the entity is valid.
     *
     * @return true if the entity's ID is not equal to the invalid entity index; false otherwise.
     */
    bool Entity::IsValid() const
    {
        return m_ID != INVALID_ENTITY_INDEX;
    }

    /**
     * @brief Destroys this entity by requesting removal from the associated EntityManager.
     *
     * After calling this method, the entity is considered invalid and should not be used.
     */
    void Entity::Destroy()
    {
        m_EntityManagerReference.DestroyEntity(*this);
    }

    /**
     * @brief Implicitly converts the Entity to its underlying ID.
     *
     * @return The entity's unique identifier as a 32-bit unsigned integer.
     */
    Entity::operator std::uint32_t() const
    {
        return GetID();
    }
}
