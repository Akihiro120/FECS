#pragma once
#include <fecs/core/entity/entity.hpp>
#include <fecs/registry/registry.hpp>

namespace FECS
{

    template <typename Component>
    inline void Entity::AttachComponent(const Component& component)
    {
        m_RegistryReference.AttachComponent(*this, component);
    }
}
