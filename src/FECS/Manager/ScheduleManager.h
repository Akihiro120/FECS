#pragma once
#include <vector>
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
            m_SetExecutionOrder.push_back(0);
            m_FixedStep = 1.0f / 60.0f;
        }

        ~ScheduleManager()
        {
            for (auto& set : m_Sets)
            {
                for (auto& sys : set.startupSystem)
                {
                    sys.destroy(sys.object);
                }
                for (auto& sys : set.updateSystem)
                {
                    sys.destroy(sys.object);
                }
                for (auto& sys : set.fixedSystem)
                {
                    sys.destroy(sys.object);
                }
                for (auto& sys : set.timedSystems)
                {
                    sys.func.destroy(sys.func.object);
                }
            }
        }

        auto SetExecutionOrder(const std::vector<uint16_t>& order) -> void
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
                    for (const auto& setIndex : m_SetExecutionOrder)
                    {
                        if (m_Sets.size() < setIndex)
                        {
                            for (auto& sys : m_Sets[setIndex].fixedSystem)
                            {
                                sys.invoke(sys.object, m_World);
                            }
                        }
                    }
                    m_GlobalFixedAccumulator -= m_FixedStep;
                }
            }

            for (const auto& setIndex : m_SetExecutionOrder)
            {
                if (setIndex >= m_Sets.size())
                {
                    continue;
                }

                Internal::SystemSet& set = m_Sets[setIndex];

                for (auto& sys : set.updateSystem)
                {
                    sys.invoke(sys.object, m_World);
                }

                for (auto& sys : set.timedSystems)
                {
                    sys.accumulator += dt;
                    if (sys.accumulator >= sys.interval)
                    {
                        sys.func.invoke(sys.func.object, m_World);
                        sys.accumulator -= sys.interval;
                    }
                }
            }
        }

        auto RunStartup() -> void
        {
            for (const auto& setIndex : m_SetExecutionOrder)
            {
                if (setIndex >= m_Sets.size())
                {
                    continue;
                }

                Internal::SystemSet& set = m_Sets[setIndex];

                for (auto& sys : set.startupSystem)
                {
                    sys.invoke(sys.object, m_World);
                }

                set.startupSystem.clear();
            }
        }

        template <typename Func>
        auto RegisterSystem(
            Func&& func,
            uint16_t setIndex,
            SystemMode mode,
            float interval = 0.0f) -> void
        {
            if (setIndex >= m_Sets.size())
                m_Sets.resize(setIndex + 1);

            SystemSet& set = m_Sets[setIndex];

            using Fn = std::decay_t<Func>;
            Fn* stored = new Fn(std::forward<Fn>(func));

            SystemEntry callable{
                [](void* obj, World& w)
            { (*static_cast<Fn*>(obj))(w); },
                [](void* obj)
            { delete static_cast<Fn*>(obj); },
                stored,
            };

            switch (mode)
            {
            case SystemMode::STARTUP:
                set.startupSystem.push_back(callable);
                break;
            case SystemMode::UPDATE:
                set.updateSystem.push_back(callable);
                break;
            case SystemMode::FIXED:
                set.fixedSystem.push_back(callable);
                break;
            case SystemMode::TIMED:
                set.timedSystems.push_back({callable, interval, 0.0f});
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

        std::vector<SystemSet> m_Sets;
        std::vector<uint16_t> m_SetExecutionOrder;
    };
}

namespace FECS::Builder
{
    template <typename... Args>
    template <typename Func>
    auto SystemBuilder<Args...>::RegisterToScheduler(Func&& func) -> void
    {
        m_ScheduleManager.RegisterSystem(func, m_SetIndex, m_Mode, m_Interval);
    }
}
