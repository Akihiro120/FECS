#pragma once
#include "fecs/containers/sparse_set.h"
#include "fecs/core/types.h"
#include <fecs/manager/component_manager.h>
#include <fecs/manager/entity_manager.h>
#include <array>
#include <algorithm>
#include <tuple>
#include <utility>

namespace FECS
{
    namespace Manager
    {
        template <typename... Components>
        class View
        {
            static_assert(sizeof...(Components) >= 1, "View requires at least one component type");

        public:
            View(EntityManager* entityManagerPointer)
                : m_Pools(&ComponentManager::GetPool<Components>(entityManagerPointer)...),
                  m_LastVersions{},
                  m_GlobalVersion(false),
                  m_CacheBuilt(false)
            {
                m_Cache = {};
                m_Cache.clear();
            }

            ~View()
            {
            }

            template <typename Func>
            void Each(Func&& fn)
            {
                bool dirty = !m_CacheBuilt;
                std::size_t idx = 0;
                ((dirty |=
                  (ComponentManager::GetVersion<Components>() != m_LastVersions[idx++])),
                 ...);
                ((dirty |=
                  ComponentManager::GetVersion<GlobalComponent>() != m_GlobalVersion));

                // its dirty
                if (dirty)
                {
                    m_Cache.clear();

                    // rebuild
                    auto sizes = BuildSizes(std::make_index_sequence<N>{});

                    std::size_t driverIdx = 0;
                    std::size_t minSize = sizes[0];

                    for (std::size_t i = 1; i < N; i++)
                    {
                        if (sizes[i] < minSize)
                        {
                            minSize = sizes[i];
                            driverIdx = i;
                        }
                    }

                    DispatchDriver(driverIdx, std::forward<Func>(fn), std::make_index_sequence<N>{});

                    // declare clean
                    idx = 0;
                    ((m_LastVersions[idx++] = ComponentManager::GetVersion<Components>()), ...);
                    m_GlobalVersion = ComponentManager::GetVersion<GlobalComponent>();
                    m_CacheBuilt = true;
                }

                // loop over cached entities
                for (Entity e : m_Cache)
                {
                    Invoke(std::forward<Func>(fn), e, std::make_index_sequence<N>{});
                }
            }

            // reserve a predicted amount to cache
            View& Reserve(std::size_t size)
            {
                m_Cache.reserve(size);

                return *this;
            }

        private:
            template <std::size_t... I>
            inline std::array<std::size_t, sizeof...(Components)> BuildSizes(std::index_sequence<I...>) const
            {
                return {{std::get<I>(m_Pools)->Size()...}};
            }

            template <typename Func, std::size_t... I>
            void DispatchDriver(std::size_t driverIdx, Func&& func, std::index_sequence<I...>)
            {
                bool handled = ((driverIdx == I && (LoopDriver<I>(func), true)) || ...);
                (void) handled;
            }

            template <std::size_t DriverIdx, typename Func>
            void LoopDriver(Func&& func)
            {
                auto* driver = std::get<DriverIdx>(m_Pools);
                for (std::size_t i = 0, n = driver->Size(); i < n; i++)
                {
                    Entity e = driver->EntityAt(i);
                    if (!HasAll<DriverIdx>(e))
                        continue;

                    m_Cache.push_back(e);
                }
            }

            template <std::size_t DriverIdx, std::size_t... I>
            inline bool HasAllImpl(Entity e, std::index_sequence<I...>) const
            {
                return ((I == DriverIdx ? true : std::get<I>(m_Pools)->Has(e)) && ...);
            }

            template <std::size_t DriverIdx>
            inline bool HasAll(Entity e) const
            {
                return HasAllImpl<DriverIdx>(e, std::make_index_sequence<sizeof...(Components)>{});
            }

            template <typename Func, std::size_t... I>
            inline void Invoke(Func&& func, Entity e, std::index_sequence<I...>)
            {
                func(e, std::get<I>(m_Pools)->Get(e)...);
            }

            static constexpr std::size_t N = sizeof...(Components);

            using PoolsTuple = std::tuple<Container::SparseSet<Components>*...>;
            PoolsTuple m_Pools;
            bool m_CacheBuilt = false;
            std::array<size_t, N> m_LastVersions{};
            std::size_t m_GlobalVersion = 0;
            std::vector<Entity> m_Cache;
        };
    }
}
