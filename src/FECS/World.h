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
        class EntityManager;
        class ComponentManager;
        class ResourceManager;
        class ViewManager;
    }

    class World
    {
    public:
        World();
        ~World();

        auto Entities() -> Manager::EntityManager&;
        auto Components() -> Manager::ComponentManager&;
        auto View() -> Manager::ViewManager&;
        auto Resources() -> Manager::ResourceManager&;
        auto Scheduler() -> Manager::ScheduleManager&;

    private:
        // managers
        std::unique_ptr<Manager::EntityManager> p_EntityManager;
        std::unique_ptr<Manager::ComponentManager> p_ComponentManager;
        std::unique_ptr<Manager::ResourceManager> p_ResourceManager;
        std::unique_ptr<Manager::ScheduleManager> p_ScheduleManager;
        std::unique_ptr<Manager::ViewManager> p_ViewManager;
    };
}
