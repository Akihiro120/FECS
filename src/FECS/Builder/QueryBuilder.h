#pragma once
#include <limits>
#include <memory>
#include <tuple>
#include "FECS/Containers/SparseSet.h"
#include "FECS/Core/Types.h"
#include "FECS/Manager/ComponentManager.h"

namespace FECS::Builder
{
    using namespace Manager;

    template <typename... Components>
    class QueryBuilder
    {
    public:
        QueryBuilder(std::unique_ptr<ComponentManager>& manager)
            : p_ComponentManager(manager),
              m_Pools(std::make_tuple(manager->GetStorages().GetPool<Components>()...))
        {
        }
        ~QueryBuilder() = default;

        template <typename Func>
        auto Each(Func queryFunction) -> void
        {
        }

        template <typename T>
        auto With() -> QueryBuilder<Components...>&
        {
            return *this;
        }

        template <typename T>
        auto Without() -> QueryBuilder<Components...>&
        {
            return *this;
        }

    private:
        using PoolTuple = std::tuple<SparseSet<Components>*...>;

        PoolTuple m_Pools;
        std::unique_ptr<ComponentManager>& p_ComponentManager;
    };
}
