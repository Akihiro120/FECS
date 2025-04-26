#pragma once
#include <fecs/core/entity/entity.hpp>
#include <fecs/registry/registry.hpp>

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
}
