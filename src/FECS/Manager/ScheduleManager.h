#pragma once
#include <functional>
#include <vector>
#include <string>
#include "FECS/Builder/SystemBuilder.h"
#include "FECS/World.h"

namespace FECS::Manager
{
    using namespace Builder;

    enum class SystemMode
    {
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
        std::vector<std::function<void(World&)>> updateSystem;
        std::vector<std::function<void(World&)>> fixedSystem;
        std::vector<TimedSystem> timedSystems;
    };

    class ScheduleManager
    {

    public:
        ScheduleManager(World& world)
            : m_World(world)
        {
            m_SetExecutionOrder.push_back("Default");
        }

        auto SetExecutionOrder(const std::vector<std::string>& order) -> void
        {
            m_SetExecutionOrder = order;
        }

        auto AddSystem() -> SystemBuilder<>
        {
            return SystemBuilder<>(m_World, *this);
        }

        auto Run() -> void
        {
        }

    private:
        auto RegisterSystem(std::function<void(World&)> func) -> void
        {
        }

    private:
        friend SystemBuilder<>;

        World& m_World;
        float m_FixedStep;
        float m_GlobalFixedAccumulator = 0.0f;

        std::unordered_map<std::string, SystemSet> m_Sets;
        std::vector<std::string> m_SetExecutionOrder;
    };
}

namespace FECS::Builder
{
    template <typename... Args>
    auto SystemBuilder<Args...>::RegisterToScheduler(std::function<void(World&)> func) -> void
    {
        m_ScheduleManager.RegisterSystem(std::move(func));
    }
}
