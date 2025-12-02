#pragma once
#include "Constants.h"
#include "Components.h"
#include "raymath.h"
#include <FECS/FECS.h>
#include <cstdint>

#define TURN_FACTOR 0.2f
#define VISUAL_RANGE 40.0f
#define PROTECTED_RANGE 20.0f

#define SEPERATION_WEIGHT 15.0f
#define ALIGNMENT_WEIGHT 0.002f
#define COHESION_WEIGHT 0.005f

#define MIN_SPEED 3
#define MAX_SPEED 6

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
    for (int i = 0; i < 1000; i++)
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
    float margin = 100.0f;
    query.Each([margin](FECS::Entity id, PositionComponent& pos, VelocityComponent& vel)
    {
        if (pos.x < margin)
        {
            vel.x = vel.x + TURN_FACTOR;
        }
        if (pos.x > SCREEN_WIDTH - margin)
        {
            vel.x = vel.x - TURN_FACTOR;
        }
        if (pos.y < margin)
        {
            vel.y = vel.y + TURN_FACTOR;
        }
        if (pos.y > SCREEN_HEIGHT - margin)
        {
            vel.y = vel.y - TURN_FACTOR;
        }
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

static auto ResolveForces(FECS::Query<PositionComponent, VelocityComponent> query) -> void
{
    query.Each([&](FECS::Entity id1, PositionComponent& pos1, VelocityComponent& vel1)
    {
        Vector2 separationForce = {0.0f, 0.0f};
        Vector2 alignmentForce = {0.0f, 0.0f};
        Vector2 cohesionForce = {0.0f, 0.0f};

        Vector2 totalVelocity = {0.0f, 0.0f};
        Vector2 centerOfMass = {0.0f, 0.0f};

        uint32_t numNeighbours = 0;
        query.Each([&](FECS::Entity id2, PositionComponent& pos2, VelocityComponent& vel2)
        {
            if (id1 != id2)
            {
                float distance = Vector2Distance(pos1, pos2);

                // seperation
                if (distance <= PROTECTED_RANGE && distance > 0.0f)
                {
                    Vector2 direction = pos1 - pos2;
                    float distanceSqr = Vector2DistanceSqr(pos1, pos2);

                    Vector2 awayVector = direction / distanceSqr;
                    separationForce += awayVector;
                }

                // alignment & cohesion
                if (distance <= VISUAL_RANGE && distance > 0.0f)
                {
                    numNeighbours++;
                    totalVelocity += vel2;
                    centerOfMass += pos2;
                }
            }
        });

        separationForce *= SEPERATION_WEIGHT;

        if (numNeighbours > 0)
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
    });
}
