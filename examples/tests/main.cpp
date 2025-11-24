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

namespace Prefabs
{
    auto PlayerPrefab(FECS::Builder::EntityBuilder& builder) -> void
    {
        builder
            .Attach<Position>(Position{32.0f, 18.0f});
    }
}

auto CreatePlayer(FECS::World& world) -> FECS::Entity
{
    FECS::Entity e = world.Entities()
                         .Create()
                         .Apply(Prefabs::PlayerPrefab)
                         .Build();

    return e;
}

auto ModifyPlayer(FECS::World& world, FECS::Entity entity) -> void
{
    world.Entities()
        .Modify(entity)
        .Patch<Position>([](Position& position)
    {
        position.x = 128;
        position.y = 256;
    });
}

auto main() -> int
{
    FECS::World world = FECS::Init();
    FECS::Entity e = CreatePlayer(world);

    std::cout << world.Components().Get<Position>(e) << std::endl;

    ModifyPlayer(world, e);
    std::cout << world.Components().Get<Position>(e) << std::endl;

    return 0;
}
