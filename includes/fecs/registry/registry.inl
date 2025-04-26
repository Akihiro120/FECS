#pragma once
#include <fecs/registry/registry.hpp>

namespace FECS
{
    template <typename Component>
    inline void Registry::AttachComponent(const Entity& entity, const Component& component)
    {
    }

    template <typename Component>
    inline void Registry::DetachComponent(const Entity& entity)
    {
    }
}
