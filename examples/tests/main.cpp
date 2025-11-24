#include "FECS/Builder/EntityBuilder.h"
#include "FECS/Manager/ComponentManager.h"
#include <iostream>
#include <FECS/FECS.h>

struct Position
{
    float x = 0.0f;
    float y = 0.0f;
};

std::ostream& operator<<(std::ostream& stream, Position& pos)
{
    return stream << "Position: {" << pos.x << ", " << pos.y << "}";
}

auto PlayerPrefab(FECS::Builder::EntityBuilder& builder) -> void
{
    builder
        .Attach<Position>(Position{32.0f, 18.0f});
}

auto CreatePlayer(FECS::World& world) -> FECS::Entity
{
    FECS::Entity e = world.Entities()
                         .Create()
                         .Apply(PlayerPrefab)
                         .Build();

    return e;
}

auto ModifyPlayer(FECS::World& world, FECS::Entity entity) -> void
{
    // world.Entities()
    //     .Modify(entity)
    //     .Attach<Position>(Position{32.0f, 32.0f})
}

auto main() -> int
{
    FECS::World world = FECS::Init();
    FECS::Entity e = CreatePlayer(world);

    Position& val = world.Components().Get<Position>(e);
    std::cout << val << std::endl;

    return 0;
}
