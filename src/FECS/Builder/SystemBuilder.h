#pragma once
#include <string>
#include "FECS/Builder/QueryBuilder.h"
#include "FECS/Internal/SystemResolver.h"
#include "FECS/World.h"
#include "FECS/Internal/SystemData.h"

namespace FECS::Builder
{
    namespace FECS::Manager
    {
        class ScheduleManager;
    }

    template <typename... Args>
    class SystemBuilder
    {
    private:
        template <typename... OtherArgs>
        friend class SystemBuilder;

    public:
        SystemBuilder(World& world, Manager::ScheduleManager& manager)
            : m_World(world),
              m_ScheduleManager(manager)
        {
        }

        template <typename T>
        auto Write() -> SystemBuilder<Args..., T&>
        {
            auto next = SystemBuilder<Args..., T&>(m_World, m_ScheduleManager);
            CopyStateTo(next);
            return next;
        }

        template <typename T>
        auto Read() -> SystemBuilder<Args..., const T&>
        {
            auto next = SystemBuilder<Args..., const T&>(m_World, m_ScheduleManager);
            CopyStateTo(next);
            return next;
        }

        template <typename... Comps>
        auto WithQuery() -> SystemBuilder<Args..., QueryBuilder<Comps...>>
        {
            auto next = SystemBuilder<Args..., QueryBuilder<Comps...>>(m_World, m_ScheduleManager);
            CopyStateTo(next);
            return next;
        }

        auto In(uint16_t setIndex) -> SystemBuilder<Args...>&
        {
            m_SetIndex = setIndex;
            return *this;
        }

        auto Startup() -> SystemBuilder<Args...>&
        {
            m_Mode = Internal::SystemMode::STARTUP;
            return *this;
        }

        auto Update() -> SystemBuilder<Args...>&
        {
            m_Mode = Internal::SystemMode::UPDATE;
            return *this;
        }

        auto Timed(float interval) -> SystemBuilder<Args...>&
        {
            m_Mode = Internal::SystemMode::TIMED;
            m_Interval = interval;
            return *this;
        }

        auto Fixed() -> SystemBuilder<Args...>&
        {
            m_Mode = Internal::SystemMode::FIXED;
            return *this;
        }

        auto CleanUp() -> SystemBuilder<Args...>&
        {
            m_Mode = Internal::SystemMode::CLEANUP;
            return *this;
        }

        template <typename Fn>
        auto Build(Fn&& func) -> void
        {
            auto task = [func](World& world)
            {
                func(Internal::Resolver<Args>::Get(world)...);
            };

            RegisterToScheduler(task);
        }

    private:
        template <typename NewBuilderType>
        auto CopyStateTo(NewBuilderType& newBuilder) -> void
        {
            newBuilder.m_SetIndex = m_SetIndex;
            newBuilder.m_Interval = m_Interval;
            newBuilder.m_Mode = m_Mode;
        }

    private:
        template <typename Func>
        auto RegisterToScheduler(Func&& func) -> void;

        World& m_World;
        float m_Interval = 0.0f;
        uint16_t m_SetIndex = 0;
        Internal::SystemMode m_Mode = Internal::SystemMode::UPDATE;

        Manager::ScheduleManager& m_ScheduleManager;
    };
}
