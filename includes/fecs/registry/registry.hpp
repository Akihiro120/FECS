#pragma once
#include <fecs/manager/entity/entity_manager.hpp>
#include <fecs/manager/component/component_manager.h>

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

        template <typename Component>
        Component& Get(const Entity& entity);

    private:
        Manager::EntityManager m_EntityManager;
        Manager::ComponentManager m_ComponentManager;
    };
}

#include "registry.inl"
