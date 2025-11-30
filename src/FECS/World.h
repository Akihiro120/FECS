#pragma once
#include <memory>
#include "FECS/Manager/EntityManager.h"
#include "FECS/Manager/ComponentManager.h"
#include "FECS/Manager/ResourceManager.h"
#include "FECS/Manager/ViewManager.h"

namespace FECS
{
    namespace Manager
    {
        class ScheduleManager;
    }

    using namespace Manager;

    class World
    {
    public:
        World();
        ~World();

        auto Entities() -> EntityManager&;
        auto Components() -> ComponentManager&;
        auto View() -> ViewManager&;
        auto Resources() -> ResourceManager&;
        auto Scheduler() -> ScheduleManager&;

    private:
        // managers
        std::unique_ptr<EntityManager> p_EntityManager;
        std::unique_ptr<ComponentManager> p_ComponentManager;
        std::unique_ptr<ResourceManager> p_ResourceManager;
        std::unique_ptr<ScheduleManager> p_ScheduleManager;
        std::unique_ptr<ViewManager> p_ViewManager;
    };
}
