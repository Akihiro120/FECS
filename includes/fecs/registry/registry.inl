#pragma once
#include <fecs/registry/registry.hpp>
#include <fecs/core/entity/entity.hpp>

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
}
