#pragma once
#include <memory>

namespace FECS
{
    namespace Manager
    {
        class EntityManager;
        class ComponentManager;
    }

    using namespace Manager;

    class World
    {
    public:
        World();
        ~World();

        auto Entities() -> EntityManager&;

        auto Components() -> ComponentManager&;

    private:
        // managers
        std::unique_ptr<EntityManager> p_EntityManager;
        std::unique_ptr<ComponentManager> p_ComponentManager;
    };
}
