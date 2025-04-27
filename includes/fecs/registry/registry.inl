#pragma once
#include <fecs/registry/registry.h>
#include <fecs/core/entity/entity.h>

namespace FECS
{
    template <typename Component>
    inline void Registry::AttachComponent(const Entity& entity, const Component& component)
    {
        m_ComponentManager.AttachComponent<Component>(entity.GetID(), component);
    }

    template <typename Component>
    inline void Registry::DetachComponent(const Entity& entity)
    {
        m_ComponentManager.DetachComponent<Component>(entity.GetID());
    }

    template <typename Component>
    inline Component& Registry::Get(const Entity& entity)
    {
        return m_ComponentManager.Get<Component>(entity.GetID());
    }

    template <typename Component>
    inline bool Registry::Has(const Entity& entity)
    {
        return m_ComponentManager.Has<Component>(entity.GetID());
    }

    template <typename... Components, typename Function>
    inline void Registry::Each(Function&& queryFunction)
    {
        m_QueryManager.Each<Components...>(queryFunction);
    }
}
