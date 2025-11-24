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

auto main() -> int
{
    FECS::World world;
    world.View()
        .Query<Position, Velocity>()
        .Each([](FECS::Entity id, Position& pos, Velocity& vel) {});

    return 0;
}
