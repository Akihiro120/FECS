#pragma once
#include <fecs/core/entity/entity.h>
#include <fecs/registry/registry.h>

namespace FECS
{

    template <typename Component>
    inline Entity& Entity::AttachComponent(const Component& component)
    {
        m_RegistryReference.AttachComponent<Component>(*this, component);
        return *this;
    }

    template <typename Component>
    inline Entity& Entity::DetachComponent()
    {
        m_RegistryReference.DetachComponent<Component>(*this);
        return *this;
    }

    template <typename Component>
    inline Component& Entity::Get()
    {
        return m_RegistryReference.Get<Component>(*this);
    }

    template <typename Component>
    inline bool Entity::Has()
    {
        return m_RegistryReference.Has<Component>(*this);
    }
}
