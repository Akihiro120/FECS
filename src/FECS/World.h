#pragma once
#include <memory>
#include "FECS/Manager/EntityManager.h"
#include "FECS/Manager/ComponentManager.h"

namespace FECS
{
    using namespace Manager;

    class World
    {
    public:
        World()
        {
            p_ComponentManager = std::make_shared<ComponentManager>();
            p_EntityManager = std::make_unique<EntityManager>(p_ComponentManager);
        }

        ~World() = default;

        auto Entities() -> EntityManager&
        {
            return *p_EntityManager;
        }

        // auto Components() -> ComponentManager&
        // {
        //     return *p_ComponentManager;
        // }

    private:
        // managers
        std::unique_ptr<EntityManager> p_EntityManager;
        std::shared_ptr<ComponentManager> p_ComponentManager;
    };
}
