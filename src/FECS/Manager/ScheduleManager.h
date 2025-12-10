#pragma once
#include "FECS/Builder/SystemBuilder.h"
#include "FECS/Internal/SystemData.h"
#include "FECS/World.h"
#include <algorithm>
#include <cstddef>
#include <map>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace FECS::Manager {
    class ScheduleManager
    {

    public:
        ScheduleManager(World &world) : m_World(world)
        {
            m_SetExecutionOrder.push_back(0);
            m_FixedStep = 1.0f / 60.0f;
        }

        ~ScheduleManager()
        {
            for (auto &set : m_Sets) {
                for (auto &sys : set.startupSystem) {
                    sys.destroy(sys.object);
                }
                for (auto &sys : set.updateSystem) {
                    sys.destroy(sys.object);
                }
                for (auto &sys : set.fixedSystem) {
                    sys.destroy(sys.object);
                }
                for (auto &sys : set.timedSystems) {
                    sys.func.destroy(sys.func.object);
                }
            }
        }

        auto SetExecutionOrder(const std::vector<uint16_t> &order) -> void
        {
            m_SetExecutionOrder = order;
        }

        auto AddSystem() -> Builder::SystemBuilder<>
        {
            return Builder::SystemBuilder<>(m_World, *this);
        }

        auto Run(float dt) -> void
        {
            m_GlobalFixedAccumulator += dt;

            if (m_FixedStep > 0.0f)
            {
                while (m_GlobalFixedAccumulator >= m_FixedStep)
                {
                    for (const auto &setIndex : m_SetExecutionOrder) {
                        if (setIndex < m_Sets.size())
                        {
                            for (auto &sys : m_Sets[setIndex].fixedSystem) {
                                sys.invoke(sys.object, m_World);
                            }
                        }
                    }
                    m_GlobalFixedAccumulator -= m_FixedStep;
                }
            }

            for (const auto &setIndex : m_SetExecutionOrder) {
                if (setIndex >= m_Sets.size())
                {
                    continue;
                }

                Internal::SystemSet &set = m_Sets[setIndex];

                for (auto &sys : set.updateSystem) {
                    sys.invoke(sys.object, m_World);
                }

                for (auto &sys : set.timedSystems) {
                    sys.accumulator += dt;
                    while (sys.accumulator >= sys.interval)
                    {
                        sys.func.invoke(sys.func.object, m_World);
                        sys.accumulator -= sys.interval;
                    }
                }
            }
        }

        auto RunStartup() -> void
        {
            for (const auto &setIndex : m_SetExecutionOrder) {
                if (setIndex >= m_Sets.size())
                {
                    continue;
                }

                Internal::SystemSet &set = m_Sets[setIndex];

                for (auto &sys : set.startupSystem) {
                    sys.invoke(sys.object, m_World);
                    sys.destroy(sys.object);
                }

                set.startupSystem.clear();
            }
        }

        auto RunCleanUp() -> void
        {
            for (const auto &setIndex : m_SetExecutionOrder) {
                if (setIndex >= m_Sets.size())
                {
                    continue;
                }

                Internal::SystemSet &set = m_Sets[setIndex];

                for (auto &sys : set.cleanupSystem) {
                    sys.invoke(sys.object, m_World);
                    sys.destroy(sys.object);
                }

                set.cleanupSystem.clear();
            }
        }

        template <typename Func>
        auto RegisterSystem(
            Func &&func, uint16_t setIndex,
            Internal::SystemMode mode,
            float interval,
            const std::string &name,
            std::vector<std::string> afterSystems,
            std::vector<std::string> beforeSystems) -> void
        {
            if (setIndex >= m_Sets.size())
            {
                m_Sets.resize(setIndex + 1);
            }

            using Fn = std::decay_t<Func>;
            Fn* stored = new Fn(std::forward<Fn>(func));

            Internal::SystemEntry callable{
                [](void* obj, World& w)
                {
                    (*static_cast<Fn*>(obj))(w);
                },
                [](void* obj)
                {
                    delete static_cast<Fn*>(obj);
                },
                stored,
            };

            // Store in Staged list instead of Sets directly
            m_Staged.emplace_back(Internal::StagedSystem{
                callable,
                setIndex,
                mode,
                interval,
                name,
                std::move(afterSystems),
                std::move(beforeSystems)
            });

            m_Baked = false;
        }

        auto SetFixedStep(float step) -> void
        {
            m_FixedStep = step;
        }

        auto Bake() -> void
        {
            if (m_Baked)
                return;

            // 1. Clear existing sets to rebuild them
            for(auto& set : m_Sets)
            {
                set.startupSystem.clear();
                set.updateSystem.clear();
                set.fixedSystem.clear();
                set.timedSystems.clear();
            }

            // 2. Group staged systems by (SetIndex, Mode)
            // We use a vector of indices to avoid moving the heavy StagedSystem objects around
            struct GroupKey { uint16_t set; Internal::SystemMode mode;
                              auto operator<=>(const GroupKey&) const = default;};

            std::map<GroupKey, std::vector<size_t> > groups;

            for (size_t i = 0; i < m_Staged.size(); ++i)
            {
                groups[{m_Staged[i].setIndex, m_Staged[i].mode}].push_back(i);
            }

            // 3. Sort and distribute
            for (auto& [key, indices] : groups)
            {
                auto sortedIndices = TopologicalSort(indices);

                Internal::SystemSet& targetSet = m_Sets[key.set];

                for (size_t idx : sortedIndices)
                {
                    // Move the entry from stage to final storage
                    // Note: We copy the entry struct, ownership of 'stored' pointer remains safe
                    auto& stage = m_Staged[idx];

                    switch (key.mode)
                    {
                    case Internal::SystemMode::STARTUP:
                        targetSet.startupSystem.push_back(stage.entry);
                        break;
                    case Internal::SystemMode::CLEANUP:
                        targetSet.cleanupSystem.push_back(stage.entry);
                        break;
                    case Internal::SystemMode::UPDATE:
                        targetSet.updateSystem.push_back(stage.entry);
                        break;
                    case Internal::SystemMode::FIXED:
                        targetSet.fixedSystem.push_back(stage.entry);
                        break;
                    case Internal::SystemMode::TIMED:
                        targetSet.timedSystems.push_back({stage.entry, stage.interval, 0.0f});
                        break;
                    }
                }
            }

            // Clear staged now that ownership is transferred to m_Sets
            m_Staged.clear();
            m_Baked = true;
        }

    private:
        auto TopologicalSort(const std::vector<size_t>& subsetIndices) -> std::vector<size_t>
        {
            if (subsetIndices.empty())
                return {};

            std::unordered_map<std::string, size_t> nameToIndex;
            std::unordered_map<size_t, std::vector<size_t> > adjacency;
            std::unordered_map<size_t, int> inDegree;

            // Initialize
            for (size_t idx : subsetIndices)
            {
                const auto& sys = m_Staged[idx];
                if (!sys.name.empty())
                {
                    nameToIndex[sys.name] = idx;
                }
                inDegree[idx] = 0;
            }

            // Build Graph
            for (size_t idx : subsetIndices)
            {
                const auto& sys = m_Staged[idx];

                // Handle "After": Target -> Current
                for (const auto& targetName : sys.after)
                {
                    if (auto it = nameToIndex.find(targetName); it != nameToIndex.end())
                    {
                        adjacency[it->second].push_back(idx);
                        inDegree[idx]++;
                    }
                }

                // Handle "Before": Current -> Target
                for (const auto& targetName : sys.before)
                {
                    if (auto it = nameToIndex.find(targetName); it != nameToIndex.end())
                    {
                        adjacency[idx].push_back(it->second);
                        inDegree[it->second]++;
                    }
                }
            }

            // Kahn's Algorithm
            std::queue<size_t> zeroDegreeQueue;
            for (size_t idx : subsetIndices)
            {
                if (inDegree[idx] == 0)
                {
                    zeroDegreeQueue.push(idx);
                }
            }

            std::vector<size_t> sortedOrder;
            sortedOrder.reserve(subsetIndices.size());

            while (!zeroDegreeQueue.empty())
            {
                size_t u = zeroDegreeQueue.front();
                zeroDegreeQueue.pop();
                sortedOrder.push_back(u);

                if (adjacency.contains(u))
                {
                    for (size_t v : adjacency[u])
                    {
                        inDegree[v]--;
                        if (inDegree[v] == 0)
                        {
                            zeroDegreeQueue.push(v);
                        }
                    }
                }
            }

            if (sortedOrder.size() != subsetIndices.size())
            {
                std::stringstream ss;
                ss << "Dependency cycle detected in system set. Involved systems: ";
                bool first = true;
                for (size_t idx : subsetIndices) {
                    if (inDegree[idx] > 0)
                    {
                        if (!first)
                        {
                            ss << ", ";
                        }
                        ss << "'" << m_Staged[idx].name << "'";
                        first = false;
                    }
                }
                throw std::runtime_error(ss.str());
            }

            return sortedOrder;
        }

    private:
        World &m_World;
        float m_FixedStep = 0.0f;
        float m_GlobalFixedAccumulator = 0.0f;
        bool m_Baked = false;

        std::vector<Internal::SystemSet> m_Sets;
        std::vector<Internal::StagedSystem> m_Staged;
        std::vector<uint16_t> m_SetExecutionOrder;
    };
}

namespace FECS::Builder {
    template <typename ... Args>
    template <typename Func>
    auto SystemBuilder<Args...>::RegisterToScheduler(Func &&func) -> void
    {
        m_ScheduleManager.RegisterSystem(func, m_SetIndex, m_Mode, m_Interval, m_Name,
            m_After, m_Before);
    }
}
