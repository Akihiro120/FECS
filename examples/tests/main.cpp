#include <iostream>
#include <FECS/FECS.h>

struct Position
{
    float x;
    float y;
};

std::ostream& operator<<(std::ostream& stream, Position& pos)
{
    return stream << "Position: x:" << pos.x << ", y:" << pos.y;
}

struct Velocity
{
    float x;
    float y;
};

std::ostream& operator<<(std::ostream& stream, Velocity& vel)
{
    return stream << "Velocity: x:" << vel.x << ", y:" << vel.y;
}

#define MAX_ENTITIES 5
auto ConstructEntities(FECS::World& world) -> void
{
    for (int i = 0; i < MAX_ENTITIES; i++)
    {
        world.Entities()
            .Create()
            .Attach(Position{(float) i, (float) i})
            .When(i % 2 == 0, [&i](FECS::Builder::EntityBuilder& builder)
        {
            builder
                .Attach(Velocity{(float) i * 32.0f, (float) i * 32.0f})
                .Patch<Position>([&i](Position& pos)
            {
                pos.x = (float) i * 16.0f;
                pos.y = (float) i * 16.0f;
            });
        }).Build();
    }
}

auto main() -> int
{
    FECS::World world;
    ConstructEntities(world);

    std::cout << "Position Entities" << std::endl;
    world.View()
        .Query<Position>()
        .Each([](FECS::Entity id, Position& pos)
    {
        std::cout << id << " " << pos << std::endl;
    });

    std::cout << std::endl
              << "Position & Velocity Entities" << std::endl;
    world.View()
        .Query<Position, Velocity>()
        .Each([](FECS::Entity id, Position& pos, Velocity& vel)
    {
        std::cout << id << " " << pos << std::endl;
        std::cout << id << " " << vel << std::endl;
    });

    return 0;
}
