#pragma once
#include <FECS/FECS.h>
#include <vector>
#include "Components.h"
#include "Constants.h"

struct SpatialGrid
{
    int width, height;
    float cellSize;
    std::vector<std::vector<FECS::Entity>> cells;

    SpatialGrid()
    {
        cellSize = 10;
        width = ceil(SCREEN_WIDTH / cellSize);
        height = ceil(SCREEN_HEIGHT / cellSize);
        cells.resize(width * height);

        for (auto& cell : cells)
        {
            cell.reserve(16);
        }
    }

    auto Clear() -> void
    {
        for (auto& cell : cells)
        {
            cell.clear();
        }
    }

    auto GetCellIndex(Vector2 pos)
    {
        int x = ((int) (pos.x / cellSize) + width) % width;
        int y = ((int) (pos.y / cellSize) + height) % height;
        return y * width + x;
    }

    auto Add(FECS::Entity e, Vector2 pos) -> void
    {
        cells[GetCellIndex(pos)].push_back(e);
    }
};

static auto SpatialGridSystem(SpatialGrid& grid, FECS::Query<PositionComponent> query) -> void
{
    grid.Clear();

    query.Each([&](FECS::Entity id, PositionComponent& pos)
    {
        // Wrap position
        Vector2 wrappedPos = pos;
        if (wrappedPos.x < 0)
            wrappedPos.x += SCREEN_WIDTH;
        if (wrappedPos.x >= SCREEN_WIDTH)
            wrappedPos.x -= SCREEN_WIDTH;
        if (wrappedPos.y < 0)
            wrappedPos.y += SCREEN_HEIGHT;
        if (wrappedPos.y >= SCREEN_HEIGHT)
            wrappedPos.y -= SCREEN_HEIGHT;

        grid.Add(id, wrappedPos);
    });
}
