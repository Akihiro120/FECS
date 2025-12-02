#pragma once
#include "FECS/Builder/QueryBuilder.h"
#include <memory>

namespace FECS::Manager
{
    class ViewManager
    {
    public:
        ViewManager(std::unique_ptr<ComponentManager>& manager)
            : p_ComponentManager(manager.get())
        {
        }

        ~ViewManager() = default;

        template <typename... Components>
        auto Query() -> Builder::QueryBuilder<Components...>
        {
            return Builder::QueryBuilder<Components...>(p_ComponentManager);
        }

    private:
        ComponentManager* p_ComponentManager;
    };
};
