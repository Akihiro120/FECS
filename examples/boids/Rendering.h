#pragma once
#include <raylib.h>
#include <FECS/FECS.h>
#include <FECS/Builder/QueryBuilder.h>
#include "Components.h"
#include "Constants.h"

static auto PrepareRaylib() -> void
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Boids");
    // SetTargetFPS(60);
}

static Vector2 s_TrianglePoints[3] = {
    {12, 0},
    {-6, -5},
    {-6, 5}};

static Vector2 RotatePoint(Vector2 point, Vector2 pos, float angle)
{
    Vector2 result;
    result.x = point.x * cos(angle) - point.y * sin(angle);
    result.y = point.x * sin(angle) + point.y * cos(angle);

    result.x += pos.x;
    result.y += pos.y;
    return result;
}

static auto Render(FECS::Query<RenderComponent, PositionComponent, VelocityComponent> query) -> void
{
    BeginDrawing();
    ClearBackground(BLACK);

    DrawFPS(10, 10);

    // render boids
    query.Each([&](FECS::Entity id, RenderComponent& render, PositionComponent& pos, VelocityComponent& vel)
    {
        float angle = atan2(vel.y, vel.x);
        Vector2 rotatedTri[3];
        for (int i = 0; i < 3; i++)
        {
            rotatedTri[i] = RotatePoint(s_TrianglePoints[i], pos, angle);
        }
        DrawTriangle(rotatedTri[0], rotatedTri[1], rotatedTri[2], render.color);
    });

    EndDrawing();
}
