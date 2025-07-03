#include "fecs/core/types.h"
#include "fecs/registry/registry.h"
#include <iostream>
#include <fecs/fecs.h>

struct Position
{
    float x, y;
};
struct Velocity
{
    float dx, dy;
};
struct Health
{
    int hp;
};

int main()
{
    FECS::Registry registry;
    registry.RegisterComponent<Position>();
    registry.RegisterComponent<Velocity>();
    registry.RegisterComponent<Health>();

    FECS::Entity e1 = registry.CreateEntity();
    registry.Attach<Position>(e1, {1.0f, 2.0f});
    registry.Attach<Velocity>(e1, {0.1f, 0.2f});
    registry.Attach<Health>(e1, {100});

    FECS::Entity e2 = registry.CreateEntity();
    registry.Attach<Position>(e2, {3.0f, 4.0f});
    registry.Attach<Velocity>(e2, {0.0f, 1.0f});

    FECS::Entity e3 = registry.CreateEntity();
    registry.Attach<Position>(e3, {5.0f, 6.0f});

    // simple view, iterate Position + Velocity for all entities
    std::cout << "All moving entities:" << std::endl;
    registry.View<Position, Velocity>()
        .Each([&](FECS::Entity e, Position& p, Velocity& v)
    {
        std::cout << "Entity " << e << " pos=(" << p.x << "," << p.y << ")"
                  << " vel=(" << v.dx << "," << v.dy << ")" << std::endl;
    });

    // view with filter: only those with Health
    std::cout << "Healthy Movers" << std::endl;
    registry.View<Position, Velocity>()
        .With<Health>()
        .Each([&](FECS::Entity e, Position& p, Velocity& v)
    {
        int hp = registry.Get<Health>(e).hp;
        std::cout << "Entity " << e << " has HP=" << hp << std::endl;
    });

    // view with exclusion: exclude those with Health
    std::cout << "Movers without Health" << std::endl;
    registry.View<Position, Velocity>()
        .Without<Health>()
        .Each([&](FECS::Entity e, Position& p, Velocity& v)
    {
        std::cout << "Entity " << e << " has no health." << std::endl;
    });
}
