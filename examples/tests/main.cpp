#include <FECS/FECS.h>

struct Position
{
    float x = 0.0f;
    float y = 0.0f;
};

auto CreatePlayer(FECS::World& world) -> void
{
    FECS::Entity e = world.Entities().Create();
}

auto main() -> int
{
    FECS::World world = FECS::Init();

    return 0;
}
