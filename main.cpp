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
    registry.RegisterComponent<std::size_t>().Reserve(amount);

    // create a list of entities
    std::vector<FECS::Entity> entities;
    entities.reserve(amount);
    for (std::uint32_t i = 0; i < amount; i++)
    {
        entities.push_back(registry.CreateEntity());
    }

    {
        Benchmark timer("FECS Attach(2 Components)");

        for (std::uint32_t i = 0; i < amount; i++)
        {
            registry.Attach<std::uint32_t>(entities[i], i * 32);
        }

        for (std::uint32_t i = 0; i < amount; i++)
        {
            registry.Attach<std::size_t>(entities[i], i);
        }
    }

    {
        Benchmark timer("FECS Get");

        for (std::uint32_t i = 0; i < amount; i++)
        {
            auto& data = registry.Get<std::uint32_t>(entities[i]);
        }
    }

    {
        auto view = registry.View<std::uint32_t, std::size_t>().Reserve(amount);
        Benchmark timer("FECS Group(2 Components)");

        view.Each([&](FECS::Entity e, std::uint32_t& x, std::size_t& y)
        {
            x += y;
        });
    }

    return 0;
}
