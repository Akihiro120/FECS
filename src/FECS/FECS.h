#pragma once
#include "FECS/World.h"
#include "FECS/Manager/ScheduleManager.h"
#include "FECS/Manager/ComponentManager.h"
#include "FECS/Manager/EntityManager.h"
#include "FECS/Manager/ResourceManager.h"
#include "FECS/Manager/ViewManager.h"
#include "FECS/Builder/EntityBuilder.h"
#include "FECS/Builder/QueryBuilder.h"
#include "FECS/Builder/SystemBuilder.h"

namespace FECS
{
    template <typename... T>
    using Query = FECS::Builder::QueryBuilder<T...>;
    using EntityBuilder = FECS::Builder::EntityBuilder;
}
