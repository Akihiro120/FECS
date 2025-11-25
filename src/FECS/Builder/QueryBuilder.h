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
            : p_ComponentManager(manager)
        {
            // construct the query
        }
        ~QueryBuilder() = default;

        template <typename Func>
        auto Each(Func queryFunction) -> void
        {
            // get all the pools type erased via a tuple
            auto dataPool = std::make_tuple(static_cast<SparseSet<Components>*>(
                p_ComponentManager->GetStorages().GetPool<Components>())...);
        }

    private:
        template <typename Tuple, std::size_t... Is>
        auto GetDriverSequence() -> void
        {
        }

        auto GetDriver() -> void
        {
        }

        std::unique_ptr<ComponentManager>& p_ComponentManager;
    };
}
