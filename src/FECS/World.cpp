#include "FECS/World.h"
#include "FECS/Manager/ScheduleManager.h"
#include "FECS/Manager/ComponentManager.h"
#include "FECS/Manager/EntityManager.h"
#include "FECS/Manager/ResourceManager.h"
#include "FECS/Manager/ViewManager.h"

namespace FECS
{
    World::World()
    {
        p_ComponentManager = std::make_unique<ComponentManager>();
        p_EntityManager = std::make_unique<EntityManager>(p_ComponentManager);
        p_ResourceManager = std::make_unique<ResourceManager>();
        p_ScheduleManager = std::make_unique<ScheduleManager>(*this);
        p_ViewManager = std::make_unique<ViewManager>(p_ComponentManager);
    }

    World::~World() = default;

    auto World::Scheduler() -> ScheduleManager&
    {
        return *p_ScheduleManager;
    }

    auto World::Entities() -> EntityManager&
    {
        return *p_EntityManager;
    }

    auto World::Components() -> ComponentManager&
    {
        return *p_ComponentManager;
    }

    auto World::View() -> ViewManager&
    {
        return *p_ViewManager;
    }

    auto World::Resources() -> ResourceManager&
    {
        return *p_ResourceManager;
    }
}
