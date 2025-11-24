#pragma once
#include "FECS/Builder/QueryBuilder.h"
#include <memory>

namespace FECS::Manager
{
    using namespace Builder;

    class ViewManager
    {
    public:
        ViewManager(std::unique_ptr<ComponentManager>& manager)
            : p_ComponentManager(manager)
        {
        }

        ~ViewManager() = default;

        template <typename... Components>
        auto Query() -> QueryBuilder<Components...>
        {
            return QueryBuilder<Components...>(p_ComponentManager);
        }

    private:
        std::unique_ptr<ComponentManager>& p_ComponentManager;
    };
};
