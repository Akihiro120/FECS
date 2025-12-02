#pragma once
#include "Constants.h"
#include "Components.h"
#include "FECS/World.h"
#include "Grid.h"
#include "raymath.h"
#include <FECS/FECS.h>
#include <cstdint>

#define PROTECTED_RANGE 8.0f
#define VISUAL_RANGE 15.0f

#define SEPERATION_WEIGHT 5.0f
#define ALIGNMENT_WEIGHT 0.05f
#define COHESION_WEIGHT 0.005f

#define MIN_SPEED 2.0f
#define MAX_SPEED 4.0f

static Color RandomPresetColor()
{
    Color presets[] = {RED, GREEN, BLUE, YELLOW, ORANGE, PURPLE, PINK, SKYBLUE, LIME, GOLD};
    int count = sizeof(presets) / sizeof(presets[0]);
    return presets[GetRandomValue(0, count - 1)];
}

static float GetRandomFloat(float min, float max)
{
    return min + (max - min) * ((float) rand() / (float) RAND_MAX);
}

static auto SpawnBoid(FECS::World& world) -> void
{
    for (int i = 0; i < 10000; i++)
    {
        Vector2 pos = {
            (float) GetRandomValue(0, SCREEN_WIDTH),
            (float) GetRandomValue(0, SCREEN_HEIGHT)};
        Color color = RandomPresetColor();

        float angle = GetRandomFloat(0.0f, 2.0f * PI);
        Vector2 vel = {cosf(angle) * MIN_SPEED, sinf(angle) * MIN_SPEED};

        world.Entities()
            .Create()
            .Attach<RenderComponent>({color})
            .Attach<PositionComponent>({pos.x, pos.y})
            .Attach<VelocityComponent>({vel.x, vel.y});
    }
}

static auto ResolveVelocities(FECS::Query<PositionComponent, VelocityComponent> query) -> void
{
    query.Each([&](FECS::Entity id, PositionComponent& pos, VelocityComponent& vel)
    {
        pos.x += vel.x;
        pos.y += vel.y;
    });
}

static auto ResolveWallCollision(FECS::Query<PositionComponent, VelocityComponent> query) -> void
{
    query.Each([](FECS::Entity id, PositionComponent& pos, VelocityComponent& vel)
    {
        if (pos.x < 0.0f)
            pos.x += SCREEN_WIDTH;
        if (pos.x >= SCREEN_WIDTH)
            pos.x -= SCREEN_WIDTH;

        if (pos.y < 0.0f)
            pos.y += SCREEN_HEIGHT;
        if (pos.y >= SCREEN_HEIGHT)
            pos.y -= SCREEN_HEIGHT;
    });
}

static auto LimitSpeed(Vector2 vel) -> Vector2
{
    float speed = Vector2Length(vel);
    if (speed > MAX_SPEED)
    {
        vel = Vector2Scale(vel, MAX_SPEED / speed);
    }
    if (speed < MIN_SPEED)
    {
        vel = Vector2Scale(vel, MIN_SPEED / speed);
    }
    return vel;
}

static auto ResolveForces(SpatialGrid& grid, FECS::World& world, FECS::Query<PositionComponent, VelocityComponent> query) -> void
{
    query.Each([&](FECS::Entity id1, PositionComponent& pos1, VelocityComponent& vel1)
    {
        Vector2 separationForce = {0.0f, 0.0f};
        Vector2 alignmentForce = {0.0f, 0.0f};
        Vector2 cohesionForce = {0.0f, 0.0f};

        Vector2 totalVelocity = {0.0f, 0.0f};
        Vector2 centerOfMass = {0.0f, 0.0f};

        uint32_t numNeighbours = 0;
        int cellX = (int) (pos1.x / grid.cellSize);
        int cellY = (int) (pos1.y / grid.cellSize);

        for (int offsetY = -1; offsetY <= 1; offsetY++)
        {
            for (int offsetX = -1; offsetX <= 1; offsetX++)
            {
                int nx = (cellX + offsetX + grid.width) % grid.width;
                int ny = (cellY + offsetY + grid.height) % grid.height;
                int neighborCell = ny * grid.width + nx;

                for (FECS::Entity id2 : grid.cells[neighborCell])
                {
                    if (id1 == id2)
                        continue;

                    PositionComponent& pos2 = world.Components().Get<PositionComponent>(id2);
                    VelocityComponent& vel2 = world.Components().Get<VelocityComponent>(id2);

                    float distanceSqr = Vector2DistanceSqr(pos1, pos2);

                    // separation
                    if (distanceSqr <= PROTECTED_RANGE * PROTECTED_RANGE && distanceSqr > 0)
                    {
                        Vector2 direction = pos1 - pos2;
                        separationForce += direction / distanceSqr;
                    }

                    // alignment & cohesion
                    if (distanceSqr <= VISUAL_RANGE * VISUAL_RANGE && distanceSqr > 0)
                    {
                        numNeighbours++;
                        totalVelocity += vel2;
                        centerOfMass += pos2;
                    }
                }
            }
        }
        separationForce *= SEPERATION_WEIGHT;

        if (numNeighbours > 3)
        {
            totalVelocity /= numNeighbours;
            centerOfMass /= numNeighbours;

            alignmentForce = Vector2Scale(totalVelocity - vel1, ALIGNMENT_WEIGHT);

            Vector2 vDesired = Vector2Normalize(centerOfMass - pos1) * MAX_SPEED;
            cohesionForce = Vector2Scale(vDesired - vel1, COHESION_WEIGHT);
        }

        Vector2 accel = separationForce + alignmentForce + cohesionForce;

        vel1 += accel;
        vel1.x += GetRandomFloat(-0.02f, 0.02f);
        vel1.y += GetRandomFloat(-0.02f, 0.02f);
        vel1 = Vector2Lerp(vel1, Vector2Normalize(vel1) * MAX_SPEED, 0.05f);
        vel1 = LimitSpeed(vel1);

        Vector2 wander = Vector2{GetRandomFloat(-0.1f, 0.1f), GetRandomFloat(-0.1f, 0.1f)};
        vel1 += wander;
    });
}
