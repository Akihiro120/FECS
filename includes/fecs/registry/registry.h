#pragma once
#include <fecs/manager/entity/entity_manager.h>
#include <fecs/manager/component/component_manager.h>
#include <fecs/manager/query/query_manager.h>

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

        template <typename Component>
        bool Has(const Entity& entity);

        template <typename... Components, typename Function>
        void Each(Function&& queryFunction);

    private:
        Manager::EntityManager m_EntityManager;
        Manager::ComponentManager m_ComponentManager;
        Manager::QueryManager m_QueryManager;
    };
}

#include "registry.inl"
