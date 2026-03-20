#include "FECS/Encoding/Encoder.h"
#include "FECS/Entity.h"
#include <FECS/World/World.h>

struct Player
{

};

struct Position
{
    float x;
    float y;
};

struct Velocity
{
    float dx;
    float dy;
};

int main()
{
    FECS::World world;
    FECS::Encoder enc = world.CreateEncoder();

    // perform operations
    FECS::Entity playerID = world.SpawnEntity();
    playerID << Player{};
    playerID << Position{32.0f, 12.0f};
    playerID << Velocity{32.0f, 12.0f};

    world.Finish(std::move(enc));
    return 0;
}
