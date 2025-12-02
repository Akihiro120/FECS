#pragma once
#include <raylib.h>
#include <raymath.h>

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

    VelocityComponent& operator=(const Vector2& a)
    {
        x = a.x;
        y = a.y;
        return *this;
    }

    VelocityComponent& operator+=(const Vector2& a)
    {
        x += a.x;
        y += a.y;
        return *this;
    }
};

struct RenderComponent
{
    Color color = BLUE;
};
