#pragma once
#include <fecs/manager/entity/entity_manager.h>
#include <fecs/manager/component/component_manager.h>

namespace FECS
{

    class Registry
    {
    public:
        Registry()
        {
        }

        ~Registry()
        {
        }

        Entity CreateEntity()
        {
            return m_EntityManager.CreateEntity();
        }

        void DestroyEntity(Entity& entity)
        {
            m_EntityManager.DestroyEntity(entity);
        }

        template <typename Component>
        void RegisterComponent()
        {
            m_ComponentManager.RegisterComponent<Component>();
        }

        template <typename Component>
        void AttachComponent(Entity entity, Component&& component)
        {
            m_ComponentManager.AttachComponent<Component>(entity, std::move(component));
        }

        template <typename Component>
        void DetachComponent(Entity entity)
        {
            m_ComponentManager.DetachComponent<Component>(entity);
        }

        template <typename Component>
        Component* GetComponent(Entity entity)
        {
            return m_ComponentManager.Get<Component>(entity);
        }

        template <typename... Components, typename Function>
        void Each(Function&& queryFunction)
        {
            m_ComponentManager.Each<Components...>(std::move(queryFunction));
        }

    private:
        Manager::EntityManager m_EntityManager;
        Manager::ComponentManager m_ComponentManager;
    };
}
