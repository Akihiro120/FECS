/**
 * @file view_manager.h
 * @brief Defines the FECS::View class for iterating over entities that have specific components.
 */

#pragma once
#include "fecs/containers/sparse_set.h"
#include "fecs/core/types.h"
#include <fecs/manager/component_manager.h>
#include <fecs/manager/entity_manager.h>
#include <array>
#include <functional>
#include <tuple>
#include <utility>

namespace FECS
{
    using namespace FECS::Manager;

    /**
     * @brief A view for iterating over entities that have all specified component types.
     *
     * The View caches matching entities and automatically rebuilds the cache
     * when component versioning changes (i.e, when entities/components are added or removed)
     *
     * @tparam Components The component types to filter and access.
     */
    template <typename... Components>
    class View
    {
        static_assert(sizeof...(Components) >= 1, "View requires at least one component type");

    public:
        /**
         * @brief Constructs a View given a pointer to the Entity Manager.
         * @param entityManagerPointer Pointer to the entity manager used by the registry.
         */
        View(EntityManager* entityManagerPointer)
            : m_EntityManager(entityManagerPointer),
              m_Pools(&ComponentManager::GetPool<Components>(entityManagerPointer)...),
              m_LastVersions{},
              m_FilterPredicate{},
              m_GlobalVersion(false),
              m_CacheBuilt(false)
        {
            m_Cache = {};
            m_Cache.clear(); ///< Begin with an empty cache

            // reserve a minimum of 16 entities
            m_Cache.reserve(16);
        }

        /**
         * @brief Destructor
         */
        ~View()
        {
        }

        /**
         * @brief Add a "must have" filter for component type T.
         * @tparam T A component type that entities must have.
         * @return A new View with the filter appended.
         */
        template <typename T>
        View<Components...> With() const
        {
            // static_assert((std::is_same_v<T, Components> || ...),
            // "With<T> filter must refer to one of the existing component types or other known pools");
            View copy = *this;
            // Capture the pool pointer by value for the lambda
            auto& pool = ComponentManager::GetPool<T>(m_EntityManager);
            copy.m_FilterPredicate.emplace_back(
                [pool](Entity e)
            { return pool.Has(e); });
            copy.m_CacheBuilt = false;
            return copy;
        }

        /**
         * @brief Add a "must not have" filter for component type T.
         * @tparam T A component type that entities must NOT have.
         * @return A new View with the exclusion filter appended.
         */
        template <typename T>
        View<Components...> Without() const
        {
            // static_assert((std::is_same_v<T, Components> || ...),
            // "Without<T> filter must refer to one of the existing component types or other known pools");
            View copy = *this;
            auto& pool = ComponentManager::GetPool<T>(m_EntityManager);
            copy.m_FilterPredicate.emplace_back(
                [pool](Entity e)
            { return !pool.Has(e); });
            copy.m_CacheBuilt = false;
            return copy;
        }

        /**
         * @brief Iterates over all entities in the view, Applying a function to each.
         *
         * If any component pool has changed since the last iteration (tracking by versioning),
         * the cache of entities is rebuilt.
         *
         * @tparam Func A lambda expression with the signature `void(Entity, Components&...)`.
         * @param fn The function to apply to each matching entity and its components.
         */
        template <typename Func>
        void Each(Func&& fn)
        {
            // Check whether the component(s) has changed in version
            bool dirty = !m_CacheBuilt;
            std::size_t idx = 0;

            // Iterate through the versions of the components to determine a change.
            ((dirty |= (ComponentManager::GetVersion<Components>() != m_LastVersions[idx++])), ...);
            ((dirty |= ComponentManager::GetVersion<GlobalComponent>() != m_GlobalVersion));

            // Its dirty, rebuild the cache.
            if (dirty)
            {
                // Clear the cache
                m_Cache.clear();

                // Find the smallest pool to use as driver
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

                // Use the smallest pool to drive iteration
                DispatchDriver(driverIdx, std::forward<Func>(fn), std::make_index_sequence<N>{});

                // Declare the cache as clean, and update the version record
                idx = 0;
                ((m_LastVersions[idx++] = ComponentManager::GetVersion<Components>()), ...);
                m_GlobalVersion = ComponentManager::GetVersion<GlobalComponent>();
                m_CacheBuilt = true;
            }

            // Loop and apply function to cached entities.
            for (Entity e : m_Cache)
            {
                Invoke(std::forward<Func>(fn), e, std::make_index_sequence<N>{});
            }

            // TODO: This might add some overhead, consider in future to move towards compile time instead of runtime.
            if (!m_FilterPredicate.empty())
            {
                m_FilterPredicate.clear();
                m_CacheBuilt = false;
            }
        }

        /**
         * @brief Optionally reserves space in the internal cache for predicted number of entities
         * @param size The number of entries to reserve space for.
         * @return Reference to self (fluent interface).
         */
        View& Reserve(std::size_t size)
        {
            m_Cache.reserve(size);

            return *this;
        }

    private:
        /// Builds an array of component pool sizes.
        template <std::size_t... I>
        inline std::array<std::size_t, sizeof...(Components)> BuildSizes(std::index_sequence<I...>) const
        {
            return {{std::get<I>(m_Pools)->Size()...}};
        }

        /// Selects and dispatches the driver pool to iterate over.
        template <typename Func, std::size_t... I>
        void DispatchDriver(std::size_t driverIdx, Func&& func, std::index_sequence<I...>)
        {
            bool handled = ((driverIdx == I && (LoopDriver<I>(func), true)) || ...);
            (void) handled;
        }

        /// Iterates over the driver pool and checks if each entity exists in all other pools.
        template <std::size_t DriverIdx, typename Func>
        void LoopDriver(Func&& func)
        {
            auto* driver = std::get<DriverIdx>(m_Pools);
            for (std::size_t i = 0, n = driver->Size(); i < n; i++)
            {
                Entity e = driver->EntityAt(i);
                if (!HasAll<DriverIdx>(e))
                    continue;

                bool ok = true;
                for (auto& pred : m_FilterPredicate)
                {
                    if (!pred(e))
                    {
                        ok = false;
                        break;
                    }
                }

                if (!ok)
                    continue;
                m_Cache.push_back(e);
            }
        }

        /// Checks if an entity exists in all pools (excluding the driver)
        template <std::size_t DriverIdx, std::size_t... I>
        inline bool HasAllImpl(Entity e, std::index_sequence<I...>) const
        {
            return ((I == DriverIdx ? true : std::get<I>(m_Pools)->Has(e)) && ...);
        }

        /// Helper for invoking HasAllImpl
        template <std::size_t DriverIdx>
        inline bool HasAll(Entity e) const
        {
            return HasAllImpl<DriverIdx>(e, std::make_index_sequence<sizeof...(Components)>{});
        }

        /// Calls the provided function on the entity and all its components.
        template <typename Func, std::size_t... I>
        inline void Invoke(Func&& func, Entity e, std::index_sequence<I...>)
        {
            func(e, std::get<I>(m_Pools)->Get(e)...);
        }

        static constexpr std::size_t N = sizeof...(Components);

        using PoolsTuple = std::tuple<Container::SparseSet<Components>*...>;
        PoolsTuple m_Pools;                     ///< Tuple of pointers to each component pool
        bool m_CacheBuilt = false;              ///< Whether the entity cacheis built.
        std::array<size_t, N> m_LastVersions{}; ///< Last known versions of each component type.
        std::size_t m_GlobalVersion = 0;        ///< Version used for global invalidation.
        std::vector<Entity> m_Cache;            ///< Cached list of matching entities.
        std::vector<std::function<bool(Entity)>> m_FilterPredicate;
        EntityManager* m_EntityManager;
    };
}
