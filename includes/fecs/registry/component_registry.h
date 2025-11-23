#pragma once
#include "fecs/manager/component_manager.h"

namespace FECS::Registries
{
    class ComponentRegistry
    {
    public:
        ComponentRegistry() = default;
        ComponentRegistry(const ComponentRegistry&) = delete;
        ComponentRegistry& operator=(const ComponentRegistry&) = delete;

    private:
        Manager::ComponentManager m_ComponentManager;
    };
}
