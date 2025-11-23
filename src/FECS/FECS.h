#pragma once
#include "FECS/World.h"

namespace FECS
{
    inline auto Init() -> World
    {
        return World();
    }
}
