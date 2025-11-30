#pragma once
#include "FECS/Builder/QueryBuilder.h"
#include "FECS/Internal/SystemResolver.h"
#include "FECS/World.h"

namespace FECS::Builder
{
    namespace FECS::Manager
    {
        class ScheduleManager;
    }

    template <typename... Args>
    class SystemBuilder
    {
    public:
        SystemBuilder(World& world, ScheduleManager& manager)
            : m_World(world),
              m_ScheduleManager(manager)
        {
        }

        template <typename T>
        auto Write() -> SystemBuilder<Args..., T&>
        {
            return SystemBuilder<Args..., T&>(m_World, m_ScheduleManager);
        }

        template <typename T>
        auto Read() -> SystemBuilder<Args..., const T&>
        {
            return SystemBuilder<Args..., const T&>(m_World, m_ScheduleManager);
        }

        template <typename... Comps>
        auto WithQuery() -> SystemBuilder<Args..., QueryBuilder<Comps...>>
        {
            return SystemBuilder<Args..., QueryBuilder<Comps...>>(m_World, m_ScheduleManager);
        }

        auto Build(std::function<void(Args...)> func) -> void
        {
            auto task = [func](World& world)
            {
                func(Internal::Resolver<Args>::Get(world)...);
            };

            RegisterToScheduler(task);
        }

    private:
        auto RegisterToScheduler(std::function<void(World&)> func) -> void;

        World& m_World;
        ScheduleManager& m_ScheduleManager;
    };
}
