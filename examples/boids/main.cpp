#include <FECS/FECS.h>
#include <raylib.h>
#include <raymath.h>

using namespace FECS;

struct Position
{
    float x;
    float y;
};

struct Velocity
{
    float x;
    float y;
};

struct Time
{
    float deltaTime = 0.0f;

    auto GetDeltaTime() const -> float
    {
        return deltaTime;
    }
};

auto TimeSystem(Time& time) -> void
{
    time.deltaTime = GetFrameTime();
}

auto main() -> int
{
    InitWindow(1280, 720, "Boids Example");
    SetTargetFPS(60);

    World world;
    world.Resources().Add<Time>({});

    world.Scheduler()
        .AddSystem()
        .Write<Time>()
        .Build(TimeSystem);

    while (!WindowShouldClose())
    {
        world.Scheduler().Run();

        // render
        BeginDrawing();
        ClearBackground(BLACK);

        DrawFPS(10, 10);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
