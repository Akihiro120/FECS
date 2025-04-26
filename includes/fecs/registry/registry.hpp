#pragma once
#include <fecs/manager/entity/entity_manager.hpp>

namespace FECS
{

    class Registry
    {
    public:
        Registry();
        ~Registry();

        // entity control
        Entity CreateEntity();
        void DestroyEntity(Entity& entity);

        template <typename Component>
        void AttachComponent(const Entity& entity, const Component& component);
        template <typename Component>
        void DetachComponent(const Entity& entity);

    private:
        Manager::EntityManager m_EntityManager;
    };
}

#include "registry.inl"
