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
};

struct ComponentThree
{
    int x;
};

struct ComponentFour
{
    int x;
};

#define NUM_ENTITIES 100


// Operation	100 Entities (ms)	10,000 Entities (ms)	1,000,000 Entities (ms)
// Create Entity	0.0021	0.0604	3.4217
// Add Component	0.0145	0.1238	7.8212
// Get Component	0.0003	0.0136	1.3510
// Remove Component	0.0033	0.0549	5.9727
// Delete Entity	0.0131	0.0676	5.9905
// Query 2 Components	0.0003	0.0244	2.6986
// Get 2 Components	0.0004	0.0249	2.4843
// Query 4 Components	0.0015	0.0348	3.1150
// Get 4 Components	0.0005	0.0497	4.9651

auto Benchmark_CreateEntity(FECS::World& world, std::vector<FECS::Entity>& ids) -> void
{
    Benchmark bm("Create Entity");
    for (int i = 0; i < NUM_ENTITIES; i++)
    {
        ids[i] = world.Entities()
                     .Create()
                     .Build();
    }
}

auto Benchmark_AddComponent(FECS::World& world, const std::vector<FECS::Entity>& ids) -> void
{
    Benchmark bm("Add Component");
    for (int i = 0; i < NUM_ENTITIES; i++)
    {
        world.Components().Attach<ComponentOne>(ids[i], {});
    }
}

auto Benchmark_GetComponent(FECS::World& world, const std::vector<FECS::Entity>& ids) -> void
{
    Benchmark bm("Get Component");
    for (int i = 0; i < NUM_ENTITIES; i++)
    {
        world.Components().Get<ComponentOne>(ids[i]);
    }
}

auto Benchmark_RemoveComponent(FECS::World& world, const std::vector<FECS::Entity>& ids) -> void
{
    Benchmark bm("Remove Component");
    for (int i = 0; i < NUM_ENTITIES; i++)
    {
        world.Components().Detach<ComponentOne>(ids[i]);
    }
}

auto Benchmark_DeleteEntity(FECS::World& world, const std::vector<FECS::Entity>& ids) -> void
{
    Benchmark bm("Delete Entity");
    for (int i = 0; i < NUM_ENTITIES; i++)
    {
        world.Entities().Delete(ids[i]);
    }
}

auto Benchmark_QueryTwoComponents(FECS::World& world) -> void
{
    Benchmark bm("Query 2 components");
    world.View()
        .Query<ComponentOne, ComponentTwo>()
        .Each([](FECS::Entity id,
                 ComponentOne& one,
                 ComponentTwo& two)
    {
        one.x++;
        two.x++;
    });
}

auto Benchmark_GetTwoComponents(FECS::World& world, std::vector<FECS::Entity>& ids) -> void
{
    Benchmark bm("Get 2 components");
    for (int i = 0; i < NUM_ENTITIES; i++)
    {
        world.Components().Get<ComponentOne>(ids[i]);
        world.Components().Get<ComponentTwo>(ids[i]);
    }
}

auto Benchmark_QueryFourComponents(FECS::World& world) -> void
{
    Benchmark bm("Query 4 components");
    world.View()
        .Query<ComponentOne, ComponentTwo, ComponentThree, ComponentFour>()
        .Each([](FECS::Entity id,
                 ComponentOne& one,
                 ComponentTwo& two,
                 ComponentThree& three,
                 ComponentFour& four)
    {
        one.x++;
        two.x++;
        three.x++;
        four.x++;
    });
}

// Times: Get (4 Components)with the fast_vector.h
// 100 Entities
// 10,000 Entities
// 1,000,000 Entities
auto Benchmark_GetFourComponents(FECS::World& world, std::vector<FECS::Entity>& ids) -> void
{
    Benchmark bm("Get 4 components");
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
    std::vector<FECS::Entity> entities;
    entities.resize(NUM_ENTITIES);

    FECS::World world;

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
        world.Components().Attach<ComponentTwo>(entities[i], {1});
        world.Components().Attach<ComponentThree>(entities[i], {1});
        world.Components().Attach<ComponentFour>(entities[i], {1});
    }

    Benchmark_QueryTwoComponents(world);
    Benchmark_GetTwoComponents(world, entities);
    Benchmark_QueryFourComponents(world);
    Benchmark_GetFourComponents(world, entities);

    return 0;
}
