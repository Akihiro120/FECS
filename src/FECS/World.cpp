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
        p_ComponentManager = std::make_unique<Manager::ComponentManager>();
        p_EntityManager = std::make_unique<Manager::EntityManager>(p_ComponentManager);
        p_ResourceManager = std::make_unique<Manager::ResourceManager>();
        p_ScheduleManager = std::make_unique<Manager::ScheduleManager>(*this);
        p_ViewManager = std::make_unique<Manager::ViewManager>(p_ComponentManager);
    }

    World::~World() = default;

    auto World::Scheduler() -> Manager::ScheduleManager&
    {
        return *p_ScheduleManager;
    }

    auto World::Entities() -> Manager::EntityManager&
    {
        return *p_EntityManager;
    }

    auto World::Components() -> Manager::ComponentManager&
    {
        return *p_ComponentManager;
    }

    auto World::View() -> Manager::ViewManager&
    {
        return *p_ViewManager;
    }

    auto World::Resources() -> Manager::ResourceManager&
    {
        return *p_ResourceManager;
    }
}
