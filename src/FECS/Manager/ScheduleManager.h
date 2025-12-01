#pragma once
#include <functional>
#include <vector>
#include <string>
#include "FECS/Builder/SystemBuilder.h"
#include "FECS/Internal/SystemData.h"
#include "FECS/World.h"

namespace FECS::Manager
{
    using namespace Builder;
    using namespace Internal;

    class ScheduleManager
    {

    public:
        ScheduleManager(World& world)
            : m_World(world)
        {
            m_SetExecutionOrder.push_back("Default");
            m_FixedStep = 1.0f / 60.0f;
        }

        auto SetExecutionOrder(const std::vector<std::string>& order) -> void
        {
            m_SetExecutionOrder = order;
        }

        auto AddSystem() -> SystemBuilder<>
        {
            return SystemBuilder<>(m_World, *this);
        }

        auto Run(float dt) -> void
        {
            m_GlobalFixedAccumulator += dt;

            if (m_FixedStep > 0.0f)
            {
                while (m_GlobalFixedAccumulator >= m_FixedStep)
                {
                    for (const auto& setName : m_SetExecutionOrder)
                    {
                        if (m_Sets.find(setName) != m_Sets.end())
                        {
                            for (auto& sys : m_Sets[setName].fixedSystem)
                            {
                                sys(m_World);
                            }
                        }
                    }
                    m_GlobalFixedAccumulator -= m_FixedStep;
                }
            }

            for (const auto& setName : m_SetExecutionOrder)
            {
                if (m_Sets.find(setName) == m_Sets.end())
                {
                    continue;
                }

                Internal::SystemSet& set = m_Sets[setName];

                for (auto& sys : set.updateSystem)
                {
                    sys(m_World);
                }

                for (auto& sys : set.timedSystems)
                {
                    sys.accumulator += dt;
                    if (sys.accumulator >= sys.interval)
                    {
                        sys.func(m_World);
                        sys.accumulator -= sys.interval;
                    }
                }
            }
        }

        auto RunStartup() -> void
        {
            for (const auto& setName : m_SetExecutionOrder)
            {
                if (m_Sets.find(setName) == m_Sets.end())
                {
                    continue;
                }

                Internal::SystemSet& set = m_Sets[setName];

                for (auto& sys : set.startupSystem)
                {
                    sys(m_World);
                }

                set.startupSystem.clear();
            }
        }

        auto RegisterSystem(
            std::function<void(World&)> func,
            const std::string& setName,
            SystemMode mode,
            float interval = 0.0f) -> void
        {
            SystemSet& set = m_Sets[setName];

            switch (mode)
            {
            case SystemMode::STARTUP:
                set.startupSystem.push_back(std::move(func));
                break;
            case SystemMode::UPDATE:
                set.updateSystem.push_back(std::move(func));
                break;
            case SystemMode::FIXED:
                set.fixedSystem.push_back(std::move(func));
                break;
            case SystemMode::TIMED:
                set.timedSystems.push_back({std::move(func), interval, 0.0f});
                break;
            }
        }

        auto SetFixedStep(float step) -> void
        {
            m_FixedStep = step;
        }

    private:
        World& m_World;
        float m_FixedStep = 0.0f;
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
        m_ScheduleManager.RegisterSystem(std::move(func), m_SetName, m_Mode, m_Interval);
    }
}
