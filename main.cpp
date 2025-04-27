#include <iostream>
#include <fecs/fecs.hpp>

struct Position
{
    float x;
    float y;
};

int main()
{
    try
    {
        FECS::Registry registry;

        FECS::Entity e0 = registry.CreateEntity();

        registry.AttachComponent<std::uint32_t>(e0, 32);
        e0.AttachComponent<float>(64.0f);

        std::cout << e0.Get<float>() << std::endl;
        std::cout << e0.Get<std::uint32_t>() << std::endl;

        FECS::Entity e1 = registry.CreateEntity();
        e1.AttachComponent<Position>({12.23f, 23.5f})
            .AttachComponent<std::uint32_t>(12);

        std::cout << e1.Get<Position>().x << " " << e1.Get<Position>().y << std::endl;
        std::cout << e1.Get<std::uint32_t>() << std::endl;

        registry.Each<std::uint32_t, Position>([](std::uint32_t id, std::uint32_t& value, Position& pos)
        {
            std::cout << "Entity: " << id << std::endl;
            std::cout << pos.x << " " << pos.y << std::endl;
            std::cout << value << std::endl;
        });
    }
    catch (std::exception e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
