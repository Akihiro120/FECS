#include <FECS/FECS.h>
#include <iostream>
#include <vector>
#include "FECS/World.h"
#include "benchmark.h"

struct ComponentOne
{
    int x;
};

struct ComponentTwo
{
    int x;
    int y;
};

struct ComponentThree
{
    int x;
    int y;
    int z;
};

struct ComponentFour
{
    int x;
    int y;
    int z;
    int w;
};

#define NUM_ENTITIES 100
using namespace FECS;

// Times: Create Entities
// 100 Entities
// 10,000 Entities
// 1,000,000 Entities
auto Benchmark_CreateEntity(World& world, std::vector<Entity>& ids) -> void
{
    Benchmark bm("Create Entity");
    for (int i = 0; i < NUM_ENTITIES; i++)
    {
        ids[i] = world.Entities()
                     .Create()
                     .Build();
    }
}

// Times: Add Components
// 100 Entities
// 10,000 Entities
// 1,000,000 Entities
auto Benchmark_AddComponent(World& world, const std::vector<Entity>& ids) -> void
{
    Benchmark bm("Add Component");
    for (int i = 0; i < NUM_ENTITIES; i++)
    {
        world.Components().Attach<ComponentOne>(ids[i], {});
    }
}

// Times: Get Component
// 100 Entities
// 10,000 Entities
// 1,000,000 Entities
auto Benchmark_GetComponent(World& world, const std::vector<Entity>& ids) -> void
{
}

// Times:
// 100 Entities
// 10,000 Entities
// 1,000,000 Entities
auto Benchmark_RemoveComponent(World& world, const std::vector<Entity>& ids) -> void
{
}

// Times: Delete Entities
// 100 Entities
// 10,000 Entities
// 1,000,000 Entities
auto Benchmark_DeleteEntity(World& world, const std::vector<Entity>& ids) -> void
{
}

// Times: Query (2 Components)
// Times:
// 100 Entities
// 10,000 Entities
// 1,000,000 Entities
auto Benchmark_QueryTwoComponents(World& world) -> void
{
}

// Times: Get (2 Components)
// 100 Entities
// 10,000 Entities
// 1,000,000 Entities
auto Benchmark_GetTwoComponents(World& world) -> void
{
}

// Times: Query (4 Components)
// Times:
// 100 Entities
// 10,000 Entities
// 1,000,000 Entities
auto Benchmark_QueryFourComponents(World& world) -> void
{
}

// Times: Get (4 Components)
// 100 Entities
// 10,000 Entities
// 1,000,000 Entities
auto Benchmark_GetFourComponents(World& world) -> void
{
}

int main()
{
    std::cout << "Running with " << NUM_ENTITIES << " entities" << std::endl;
    std::vector<Entity> entities;
    entities.resize(NUM_ENTITIES);

    World world;

    Benchmark_CreateEntity(world, entities);
    Benchmark_AddComponent(world, entities);

    return 0;
}
