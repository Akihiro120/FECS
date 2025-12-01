#include "FECS/World.h"
#include "FECS/Builder/QueryBuilder.h"

namespace FECS::Internal
{
    template <typename T>
    struct Resolver;

    template <typename T>
    struct Resolver<T&>
    {
        static auto Get(World& w) -> T&
        {
            return w.Resources().Get<T>();
        }
    };

    template <typename T>
    struct Resolver<const T&>
    {
        static auto Get(World& w) -> const T&
        {
            return w.Resources().Get<T>();
        }
    };

    template <>
    struct Resolver<World&>
    {
        static auto Get(World& w) -> World&
        {
            return w;
        }
    };

    template <>
    struct Resolver<const World&>
    {
        static auto Get(World& w) -> const World&
        {
            return w;
        }
    };

    template <typename... Components>
    struct Resolver<QueryBuilder<Components...>>
    {
        static auto Get(World& w) -> QueryBuilder<Components...>
        {
            return w.View().Query<Components...>();
        }
    };
}
