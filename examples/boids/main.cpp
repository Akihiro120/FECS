#include "FECS/Builder/QueryBuilder.h"
#include <FECS/FECS.h>
#include <iostream>
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

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
auto PrepareRaylib() -> void
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Boids");
    // SetTargetFPS(60);
}

auto Render(QueryBuilder<RenderComponent, PositionComponent> query) -> void
{
    BeginDrawing();
    ClearBackground(BLACK);

    DrawFPS(10, 10);

    // render boids
    query.Each([&](Entity id, RenderComponent& render, PositionComponent& pos)
    {
        DrawRectangle(pos.x - 8, pos.y - 8, 16, 16, render.color);
    });

    EndDrawing();
}

Color RandomPresetColor()
{
    Color presets[] = {RED, GREEN, BLUE, YELLOW, ORANGE, PURPLE, PINK, SKYBLUE, LIME, GOLD};
    int count = sizeof(presets) / sizeof(presets[0]);
    return presets[GetRandomValue(0, count - 1)];
}

float GetRandomFloat(float min, float max)
{
    return min + (max - min) * ((float) rand() / (float) RAND_MAX);
}

auto SpawnBoid(World& world) -> void
{
    float speed = 2.0f;
    for (int i = 0; i < 100; i++)
    {
        Vector2 pos = {
            (float) GetRandomValue(0, SCREEN_WIDTH),
            (float) GetRandomValue(0, SCREEN_HEIGHT)};
        Color color = RandomPresetColor();

        float angle = GetRandomFloat(0.0f, 2.0f * PI);
        Vector2 vel = {cosf(angle) * speed, sinf(angle) * speed};

        world.Entities()
            .Create()
            .Attach<RenderComponent>({color})
            .Attach<PositionComponent>({pos.x, pos.y})
            .Attach<VelocityComponent>({vel.x, vel.y});
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

auto ResolveWallCollision(QueryBuilder<PositionComponent> query) -> void
{
    float margin = 0.0f;
    query.Each([margin](Entity id, PositionComponent& pos)
    {
        if (pos.x < -margin)
            pos.x += SCREEN_WIDTH + 2 * margin;
        if (pos.x > SCREEN_WIDTH + margin)
            pos.x -= SCREEN_WIDTH + 2 * margin;

        if (pos.y < -margin)
            pos.y += SCREEN_HEIGHT + 2 * margin;
        if (pos.y > SCREEN_HEIGHT + margin)
            pos.y -= SCREEN_HEIGHT + 2 * margin;
    });
}

auto Steer(Vector2 vel)
{
    return Vector2ClampValue(vel, 0.0f, 2.0f);
}

auto ResolveForces(QueryBuilder<PositionComponent, VelocityComponent> query) -> void
{
    float separationRange = 30.0f;
    float separationWeight = 150.0f;
    float alignmentWeight = 0.8f;
    float cohesionWeight = 0.2f;
    query.Each([&](Entity id1, PositionComponent& pos1, VelocityComponent& vel1)
    {
        uint32_t numNeighbours = 0;
        Vector2 avgCenterOfMass = {0.0f, 0.0f};
        Vector2 avgVelocity = {0.0f, 0.0f};
        Vector2 seperationForce = {0.0f, 0.0f};
        query.Each([&](Entity id2, PositionComponent& pos2, VelocityComponent& vel2)
        {
            if (id1 == id2)
            {
                return;
            }

            float distance = Vector2Distance(pos1, pos2);
            if (distance <= separationRange && distance > 0.0f)
            {
                numNeighbours++;

                // seperation
                Vector2 direction = Vector2Subtract(pos1, pos2);
                float distanceSqr = Vector2DistanceSqr(pos1, pos2);

                if (distanceSqr > 0.0001f)
                {
                    Vector2 repulsion = Vector2Scale(direction, separationWeight / distanceSqr);
                    seperationForce = Vector2Add(seperationForce, repulsion);
                }

                // alignment
                avgVelocity += vel2;
                avgCenterOfMass += pos2;
            }
        });

        if (numNeighbours > 0)
        {
            avgCenterOfMass /= numNeighbours;
            avgVelocity /= numNeighbours;

            Vector2 desiredCohension = Vector2Subtract(avgCenterOfMass, pos1);
            Vector2 cohesionForce = Vector2Scale(desiredCohension, cohesionWeight);

            Vector2 desiredAlignment = Vector2Subtract(avgVelocity, vel1);
            Vector2 alignmentForce = Vector2Scale(desiredAlignment, alignmentWeight);

            Vector2 steering = cohesionForce + seperationForce + alignmentForce;
            Vector2 newVel = vel1 + steering;

            vel1.x = Steer(newVel).x;
            vel1.y = Steer(newVel).y;
        }
    });
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
        .Startup()
        .Write<World>()
        .Build(SpawnBoid);

    world.Scheduler()
        .AddSystem()
        .In(SystemGroupID::PHYSICS)
        .Fixed()
        .WithQuery<PositionComponent, VelocityComponent>()
        .Build(ResolveForces);

    world.Scheduler()
        .AddSystem()
        .In(SystemGroupID::PHYSICS)
        .Fixed()
        .WithQuery<PositionComponent, VelocityComponent>()
        .Build(ResolveVelocities);

    world.Scheduler()
        .AddSystem()
        .In(SystemGroupID::PHYSICS)
        .WithQuery<PositionComponent>()
        .Build(ResolveWallCollision);

    world.Scheduler().RunStartup();
    while (!WindowShouldClose())
    {
        world.Scheduler().Run(GetFrameTime());
    }

    CloseWindow();

    return 0;
}
