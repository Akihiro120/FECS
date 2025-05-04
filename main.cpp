#include "benchmark.h"
#include "fecs/core/types.h"
#include "fecs/manager/entity_manager.h"
#include <iostream>
#include <fecs/fecs.h>

struct Position
{
    int x;
    int y;
};

int main()
{
    std::cout << "FECS Benchmark -----------------------------" << "\033[0m" << std::endl;

    std::uint32_t amount = 10000;

    FECS::Registry registry;
    registry.GetEntityManager().Reserve(amount);
    registry.RegisterComponent<std::uint32_t>().Reserve(amount);

    // create a list of entities
    std::vector<FECS::Entity> entities;
    entities.reserve(amount);
    for (std::uint32_t i = 0; i < amount; i++)
    {
        entities.push_back(registry.CreateEntity());
    }

    {
        Benchmark timer("FECS Attach");

        for (std::uint32_t i = 0; i < amount; i++)
        {
            registry.Attach<std::uint32_t>(entities[i], i * 32);
        }
    }

    {
        Benchmark timer("FECS Get");

        for (std::uint32_t i = 0; i < amount; i++)
        {
            auto& data = registry.Get<std::uint32_t>(entities[i]);
        }
    }

    auto group = registry.Group<std::uint32_t>();
    group.Rebuild();
    group.Each([&](FECS::Entity e, std::uint32_t& i)
    {
        std::cout << i << std::endl;
    });

    return 0;
}

/*
SoA + sparse-set pools for single-component speed,

Archetype tables + chunking for multi-component queries,

Compile-time templating + inline routines to eliminate runtime overhead,

Direct integer handles + version checks for safety without indirection,

Pre-allocation + optional custom allocators for smooth memory behavior,

And staging/parallel splits if you need multithreading—
*/
