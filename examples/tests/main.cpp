#include <FECS/FECS.h>

struct Position
{
    float x;
    float y;
};

struct Velocity
{
    float x;
    float y;
};

#define MAX_ENTITIES 123
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

    world.View()
        .Query<Position, Velocity>()
        .Each([](FECS::Entity id, Position& pos, Velocity& vel) {});

    return 0;
}
