#include "FECS/Internal/SystemData.h"
#include <FECS/FECS.h>
#include <raylib.h>
#include <raymath.h>
#include <iostream>

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

auto PrepareRaylib() -> void
{
    InitWindow(1280, 720, "Boids");
    SetTargetFPS(60);
}

auto Render() -> void
{
    BeginDrawing();
    ClearBackground(BLACK);

    DrawFPS(10, 10);

    EndDrawing();
}

auto main() -> int
{
    World world = Init();
    world.Scheduler().SetFixedStep(1.0f / 60.0f);
    world.Scheduler().SetExecutionOrder({"Default", "Render"});
    world.Scheduler()
        .AddSystem()
        .In("Render")
        .Startup()
        .Build(PrepareRaylib);

    world.Scheduler()
        .AddSystem()
        .In("Render")
        .Update()
        .Build(Render);

    world.Scheduler().RunStartup();
    while (!WindowShouldClose())
    {
        world.Scheduler().Run(GetFrameTime());
    }

    CloseWindow();

    return 0;
}
