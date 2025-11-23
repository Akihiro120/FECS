/**
 * @file registry.h
 * @brief Provides the central interface for managing entities and components
 */

#pragma once
#include "fecs/registry/entity_registry.h"
#include "fecs/registry/component_registry.h"

namespace FECS
{
    using namespace FECS::Manager;

    inline static FECS::Registries::EntityRegistry* Entities()
    {
        static FECS::Registries::EntityRegistry s_Registry;
        return &s_Registry;
    }

    inline static FECS::Registries::ComponentRegistry* Components()
    {
        static FECS::Registries::ComponentRegistry s_Registry;
        return &s_Registry;
    }
}
