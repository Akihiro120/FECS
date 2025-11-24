#pragma once
#include <memory>
#include "FECS/Manager/EntityManager.h"
#include "FECS/Manager/ComponentManager.h"
#include "FECS/Manager/ResourceManager.h"
#include "FECS/Manager/ScheduleManager.h"
#include "FECS/Manager/ViewManager.h"

namespace FECS
{
    using namespace Manager;

    class World
    {
    public:
        World()
        {
            p_ComponentManager = std::make_unique<ComponentManager>();
            p_EntityManager = std::make_unique<EntityManager>(p_ComponentManager);
            p_ResourceManager = std::make_unique<ResourceManager>();
            p_ScheduleManager = std::make_unique<ScheduleManager>();
            p_ViewManager = std::make_unique<ViewManager>(p_ComponentManager);
        }

        ~World() = default;

        auto Entities() -> EntityManager&
        {
            return *p_EntityManager;
        }

        auto Components() -> ComponentManager&
        {
            return *p_ComponentManager;
        }

        auto View() -> ViewManager&
        {
            return *p_ViewManager;
        }

    private:
        // managers
        std::unique_ptr<EntityManager> p_EntityManager;
        std::unique_ptr<ComponentManager> p_ComponentManager;
        std::unique_ptr<ResourceManager> p_ResourceManager;
        std::unique_ptr<ScheduleManager> p_ScheduleManager;
        std::unique_ptr<ViewManager> p_ViewManager;
    };
}
