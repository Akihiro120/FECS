#include "FECS/Builder/QueryBuilder.h"
#include <FECS/FECS.h>
#include <raylib.h>
#include <raymath.h>

using namespace FECS;

enum SystemGroupID
{
    DEFAULT = 0,
    RENDER = 1,
    PHYSICS = 2,
};

struct PositionComponent
{
    float x;
    float y;

    operator Vector2()
    {
        return {
            x, y};
    }
};

struct VelocityComponent
{
    float x;
    float y;

    operator Vector2()
    {
        return {
            x, y};
    }
};

struct RenderComponent
{
    Color color = BLUE;
};

auto PrepareRaylib() -> void
{
    InitWindow(1280, 720, "Boids");
    SetTargetFPS(60);
}

auto Render(QueryBuilder<RenderComponent, PositionComponent> query) -> void
{
    BeginDrawing();
    ClearBackground(BLACK);

    DrawFPS(10, 10);

    // render boids
    query.Each([&](Entity id, RenderComponent& render, PositionComponent& pos)
    {
        DrawRectangle(pos.x, pos.y, 32, 32, render.color);
    });

    EndDrawing();
}

Color RandomPresetColor()
{
    Color presets[] = {RED, GREEN, BLUE, YELLOW, ORANGE, PURPLE, PINK, SKYBLUE, LIME, GOLD};
    int count = sizeof(presets) / sizeof(presets[0]);
    return presets[GetRandomValue(0, count - 1)];
}

auto SpawnBoid(World& world) -> void
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();
        Color color = RandomPresetColor();

        world.Entities()
            .Create()
            .Attach<RenderComponent>({color})
            .Attach<PositionComponent>({mousePos.x - 16.0f, mousePos.y - 16.0f})
            .Attach<VelocityComponent>({5.0f, 5.0f});
    }
}

auto ResolveVelocities(QueryBuilder<PositionComponent, VelocityComponent> query) -> void
{
    query.Each([&](Entity id, PositionComponent& pos, VelocityComponent& vel)
    {
        pos.x += vel.x;
        pos.y += vel.y;
    });
}

auto ResolveForces(QueryBuilder<VelocityComponent> query) -> void
{
}

auto main() -> int
{
    World world = Init();
    world.Scheduler().SetFixedStep(1.0f / 60.0f);
    world.Scheduler().SetExecutionOrder(
        {
            SystemGroupID::DEFAULT,
            SystemGroupID::PHYSICS,
            SystemGroupID::RENDER,
        });

    world.Scheduler()
        .AddSystem()
        .In(SystemGroupID::RENDER)
        .Startup()
        .Build(PrepareRaylib);

    world.Scheduler()
        .AddSystem()
        .In(SystemGroupID::RENDER)
        .Update()
        .WithQuery<RenderComponent, PositionComponent>()
        .Build(Render);

    world.Scheduler()
        .AddSystem()
        .In(SystemGroupID::DEFAULT)
        .Update()
        .Write<World>()
        .Build(SpawnBoid);

    world.Scheduler()
        .AddSystem()
        .In(SystemGroupID::PHYSICS)
        .Fixed()
        .WithQuery<VelocityComponent>()
        .Build(ResolveForces);

    world.Scheduler()
        .AddSystem()
        .In(SystemGroupID::PHYSICS)
        .Fixed()
        .WithQuery<PositionComponent, VelocityComponent>()
        .Build(ResolveVelocities);

    world.Scheduler().RunStartup();
    while (!WindowShouldClose())
    {
        world.Scheduler().Run(GetFrameTime());
    }

    CloseWindow();

    return 0;
}
