#include <FECS/FECS.h>
#include <FECS/Builder/QueryBuilder.h>
#include <iostream>



struct Position
{
    float x;
    float y;
};

std::ostream& operator<<(std::ostream& stream, Position& pos)
{
    return stream << "Position: x:" << pos.x << ", y:" << pos.y;
}

struct Velocity
{
    float x;
    float y;
};

std::ostream& operator<<(std::ostream& stream, Velocity& vel)
{
    return stream << "Velocity: x:" << vel.x << ", y:" << vel.y;
}

struct Time
{
    float time;

    auto deltaTime() const -> float
    {
        return time;
    }
};

auto MovementSystem(const Time& time, FECS::Builder::QueryBuilder<Position, Velocity> query) -> void
{
    query.Each([&](FECS::Entity id, Position& pos, Velocity& vel)
    {
        std::cout << pos << std::endl;
        pos.x += vel.x * time.deltaTime();
        pos.y += vel.y * time.deltaTime();
    });

    query.Each([&](FECS::Entity id, Position& pos, Velocity& vel)
    {
        std::cout << pos << std::endl;
    });
}

auto main() -> int
{
    FECS::World world;
    world.Entities()
        .Create()
        .Attach<Position>({6.0f, 6.0f})
        .Attach<Velocity>({12.0f, 12.0f})
        .Build();

    world.Resources().Add<Time>({32.0f});

    world.Scheduler()
        .AddSystem()
        .Read<Time>()
        .WithQuery<Position, Velocity>()
        .Build(MovementSystem);

    world.Scheduler().Run(1.0f);

    return 0;
}
