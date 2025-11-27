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

#define NUM_ENTITIES 1000000
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
    Benchmark bm("Get Component");
    for (int i = 0; i < NUM_ENTITIES; i++)
    {
        world.Components().Get<ComponentOne>(ids[i]);
    }
}

// Times:
// 100 Entities
// 10,000 Entities
// 1,000,000 Entities
auto Benchmark_RemoveComponent(World& world, const std::vector<Entity>& ids) -> void
{
    Benchmark bm("Remove Component");
    for (int i = 0; i < NUM_ENTITIES; i++)
    {
        world.Components().Detach<ComponentOne>(ids[i]);
    }
}

// Times: Delete Entities
// 100 Entities
// 10,000 Entities
// 1,000,000 Entities
auto Benchmark_DeleteEntity(World& world, const std::vector<Entity>& ids) -> void
{
    Benchmark bm("Delete Entity");
    for (int i = 0; i < NUM_ENTITIES; i++)
    {
        world.Entities().Delete(ids[i]);
    }
}

// Times: Query (2 Components)
// Times:
// 100 Entities
// 10,000 Entities
// 1,000,000 Entities
auto Benchmark_QueryTwoComponents(World& world) -> void
{
    Benchmark bm("Query 2 components");
    world.View()
        .Query<ComponentOne, ComponentTwo>()
        .Each([](Entity id,
                 ComponentOne& one,
                 ComponentTwo& two) {});
}

// Times: Get (2 Components)
// 100 Entities
// 10,000 Entities
// 1,000,000 Entities
auto Benchmark_GetTwoComponents(World& world, std::vector<Entity>& ids) -> void
{
    Benchmark bm("Get 2 components");
    for (int i = 0; i < NUM_ENTITIES; i++)
    {
        world.Components().Get<ComponentOne>(ids[i]);
        world.Components().Get<ComponentTwo>(ids[i]);
    }
}

// Times: Query (4 Components)
// Times:
// 100 Entities
// 10,000 Entities
// 1,000,000 Entities
auto Benchmark_QueryFourComponents(World& world) -> void
{
    Benchmark bm("Query 4 components");
    world.View()
        .Query<ComponentOne, ComponentTwo, ComponentThree, ComponentFour>()
        .Each([](Entity id,
                 ComponentOne& one,
                 ComponentTwo& two,
                 ComponentThree& three,
                 ComponentFour& four) {});
}

// Times: Get (4 Components)with the fast_vector.h
// 100 Entities
// 10,000 Entities
// 1,000,000 Entities
auto Benchmark_GetFourComponents(World& world, std::vector<Entity>& ids) -> void
{
    Benchmark bm("Get 2 components");
    for (int i = 0; i < NUM_ENTITIES; i++)
    {
        world.Components().Get<ComponentOne>(ids[i]);
        world.Components().Get<ComponentTwo>(ids[i]);
        world.Components().Get<ComponentThree>(ids[i]);
        world.Components().Get<ComponentFour>(ids[i]);
    }
}

int main()
{
    std::cout << "Running with " << NUM_ENTITIES << " entities" << std::endl;
    std::vector<Entity> entities;
    entities.resize(NUM_ENTITIES);

    World world;

    Benchmark_CreateEntity(world, entities);
    Benchmark_AddComponent(world, entities);
    Benchmark_GetComponent(world, entities);
    Benchmark_RemoveComponent(world, entities);
    Benchmark_DeleteEntity(world, entities);

    for (int i = 0; i < NUM_ENTITIES; i++)
    {
        entities[i] = world.Entities()
                          .Create()
                          .Build();

        world.Components().Attach<ComponentOne>(entities[i], {1});
        world.Components().Attach<ComponentTwo>(entities[i], {1, 1});
        world.Components().Attach<ComponentThree>(entities[i], {1, 1, 1});
        world.Components().Attach<ComponentFour>(entities[i], {1, 1, 1, 1});
    }

    Benchmark_QueryTwoComponents(world);
    Benchmark_GetTwoComponents(world, entities);
    Benchmark_QueryFourComponents(world);
    Benchmark_GetFourComponents(world, entities);

    return 0;
}
