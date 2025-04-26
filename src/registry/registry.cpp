#include <fecs/registry/registry.hpp>
#include <fecs/core/entity/entity.hpp>

namespace FECS
{

    /**
     * @brief Constructs a Registry and initializes its internal EntityManager.
     *
     * Associates the EntityManager with this Registry instance to manage entity lifecycles.
     */
    Registry::Registry()
        : m_EntityManager(*this)
    {
    }

    /**
     * @brief Destroys the Registry instance.
     *
     * Cleans up resources associated with the Registry. No additional actions are performed.
     */
    Registry::~Registry()
    {
    }

    /**
     * @brief Creates and returns a new entity.
     *
     * Delegates entity creation to the internal EntityManager and returns the newly created entity.
     *
     * @return Entity The newly created entity.
     */
    Entity Registry::CreateEntity()
    {
        Entity newEntity = m_EntityManager.CreateEntity();
        return newEntity;
    }

    /**
     * @brief Destroys the specified entity and removes it from the registry.
     *
     * @param entity Reference to the entity to be destroyed.
     */
    void Registry::DestroyEntity(Entity& entity)
    {
        m_EntityManager.DestroyEntity(entity);
    }
}
