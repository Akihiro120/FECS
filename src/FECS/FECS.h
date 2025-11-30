#pragma once
#include "FECS/World.h"
#include "FECS/Manager/ScheduleManager.h"
#include "FECS/Manager/ComponentManager.h"
#include "FECS/Manager/EntityManager.h"
#include "FECS/Manager/ResourceManager.h"
#include "FECS/Manager/ViewManager.h"

namespace FECS
{
    inline auto Init() -> World
    {
        return World();
    }
}
