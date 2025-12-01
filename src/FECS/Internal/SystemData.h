#pragma once
#include <vector>
#include <functional>
#include "FECS/World.h"

namespace FECS::Internal
{
    enum class SystemMode
    {
        STARTUP,
        UPDATE,
        FIXED,
        TIMED
    };

    struct TimedSystem
    {
        std::function<void(World&)> func;
        float interval;
        float accumulator = 0.0f;
    };

    struct SystemSet
    {
        std::vector<std::function<void(World&)>> startupSystem;
        std::vector<std::function<void(World&)>> updateSystem;
        std::vector<std::function<void(World&)>> fixedSystem;
        std::vector<TimedSystem> timedSystems;
    };
}
