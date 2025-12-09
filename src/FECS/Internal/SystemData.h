#pragma once
#include <vector>
#include "FECS/World.h"

namespace FECS::Internal
{
    enum class SystemMode
    {
        STARTUP,
        CLEANUP,
        UPDATE,
        FIXED,
        TIMED
    };

    struct SystemEntry
    {
        void (*invoke)(void*, World&);
        void (*destroy)(void*);
        void* object;
    };

    struct TimedSystem
    {
        SystemEntry func;
        float interval;
        float accumulator = 0.0f;
    };

    struct SystemSet
    {
        std::vector<SystemEntry> startupSystem;
        std::vector<SystemEntry> cleanupSystem;
        std::vector<SystemEntry> updateSystem;
        std::vector<SystemEntry> fixedSystem;
        std::vector<TimedSystem> timedSystems;
    };
}
