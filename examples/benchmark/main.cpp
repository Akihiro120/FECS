#include "fecs/core/types.h"
#include <fecs/fecs.h>
#include <fecs/containers/sparse/sparse_set.h>
#include <chrono>
#include <iomanip>
#include <iostream>

struct Benchmark
{
    std::chrono::high_resolution_clock::time_point start, end;
    std::chrono::duration<float> duration;
    std::string name;
    int samples = 1;

    Benchmark(const std::string& name, int samples = 1)
    {
        this->name = name;
        this->samples = samples;
        start = std::chrono::high_resolution_clock::now();
    }

    ~Benchmark()
    {
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;

        float ms = (duration.count() * 1000.0f) / samples;
        std::cout << name;

        // Move to column 40 (or whichever position works for your needs)
        int column = 40;
        if (name.length() < column)
            std::cout << std::string(column - name.length(), ' ');

        // Then print the time
        std::cout << std::fixed << std::setprecision(4)
                  << ms << " ms" << std::endl;
    }
};

int main()
{
    // sparse set performance
    {
        FECS::Container::SparseSet<std::uint32_t> sparseSet;

        int amount = FECS::SPARSE_MINIMUM_SIZE;

        // insert
        {
            Benchmark timer("SparseSet: Insert(" + std::to_string(amount) + ")");

            for (int i = 0; i < amount; i++)
            {
                sparseSet.Insert(i, i * 32);
            }
        }

        // get
        {
            Benchmark timer("SparseSet: Get(" + std::to_string(amount) + ")");

            for (int i = 0; i < amount; i++)
            {
                sparseSet.Get(i);
            }
        }

        // remove
        {
            Benchmark timer("SparseSet: Remove(" + std::to_string(amount) + ")");

            for (int i = 0; i < amount; i++)
            {
                sparseSet.Remove(i);
            }
        }
    }

    // fecs performance
    try
    {
        {
            int amount = 1000; // FECS::ENTITY_MINIMUM;

            // entities
            FECS::Registry registry;
            registry.RegisterComponent<std::uint32_t>();
            registry.RegisterComponent<std::size_t>();

            std::vector<FECS::Entity> entities;
            entities.reserve(amount);

            {
                Benchmark timer("FECS: Entity Create(" + std::to_string(amount) + ")");

                for (int i = 0; i < amount; i++)
                {
                    entities.emplace_back(registry.CreateEntity());
                }
            }

            {
                Benchmark tier("FECS: Entity Remove(" + std::to_string(amount) + ")");

                for (int i = 0; i < amount; i++)
                {
                    registry.DestroyEntity(entities[i]);
                }
            }

            // do this stuff before hand
            entities.clear();
            for (int i = 0; i < amount; i++)
            {
                entities.emplace_back(registry.CreateEntity());
            }

            {
                Benchmark tier("FECS: Attach(" + std::to_string(amount) + ")");

                for (int i = 0; i < amount; i++)
                {
                    registry.AttachComponent<std::uint32_t>(i, i * 32);
                }
            }

            {
                Benchmark tier("FECS: Get(" + std::to_string(amount) + ")");

                for (int i = 0; i < amount; i++)
                {
                    registry.GetComponent<std::uint32_t>(i);
                }
            }

            {
                Benchmark tier("FECS: Each(" + std::to_string(amount) + ")");

                registry.Each<std::uint32_t>([&](FECS::Entity id, std::uint32_t& a) {

                });
            }

            for (int i = 0; i < amount; i++)
            {
                registry.AttachComponent<std::size_t>(i, i * 32);
            }

            {
                Benchmark tier("FECS: Get 2 Components(" + std::to_string(amount) + ")");

                for (int i = 0; i < amount; i++)
                {
                    registry.GetComponent<std::uint32_t>(i);
                    registry.GetComponent<std::size_t>(i);
                }
            }

            {
                Benchmark tier("FECS: Each 2 Components(" + std::to_string(amount) + ")");

                registry.Each<std::uint32_t, std::size_t>([&](FECS::Entity id, std::uint32_t& a, std::size_t& b) {

                });
            }

            {
                Benchmark tier("FECS: Remove(" + std::to_string(amount) + ")");

                for (int i = 0; i < amount; i++)
                {
                    registry.DetachComponent<std::uint32_t>(i);
                }
            }
        }
    }
    catch (std::exception& e)
    {
        std::cout << "\033[31m" << e.what() << "\033[0m" << std::endl;
    }
}
