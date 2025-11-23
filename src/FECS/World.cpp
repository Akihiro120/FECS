#include "FECS/World.h"
#include "FECS/Manager/EntityManager.h"
#include "FECS/Manager/ComponentManager.h"

namespace FECS
{
    World::World()
    {
        p_EntityManager = std::make_unique<EntityManager>();
        p_ComponentManager = std::make_unique<ComponentManager>();
    }

    World::~World() = default;

    auto World::Entities() -> EntityManager&
    {
        return *p_EntityManager;
    }

    auto World::Components() -> ComponentManager&
    {
        return *p_ComponentManager;
    }
}