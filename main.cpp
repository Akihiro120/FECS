#include <iostream>
#include <fecs/fecs.hpp>
#include <fecs/core/entity/entity.hpp>

int main()
{
    FECS::Registry registry;

    FECS::Entity e0 = registry.CreateEntity();

    registry.AttachComponent<std::uint32_t>(e0, 32);
    e0.AttachComponent<float>(64.0f);

    return 0;
}
