#pragma once
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
        }

    private:
        std::vector<Entity> m_EntityCache;
        std::unique_ptr<ComponentManager>& p_ComponentManager;
    };
}
