#pragma once
#include <memory>
#include "FECS/Builder/EntityBuilder.h"
#include "FECS/Manager/ComponentManager.h"
#include "FECS/Containers/EntityStorage.h"

namespace FECS::Manager
{
    class EntityManager
    {
    public:
        EntityManager(std::unique_ptr<ComponentManager>& manager)
            : p_ComponentManager(manager)
        {
        }

        auto Create() -> Builder::EntityBuilder
        {
            Entity id = m_EntityStorage.Create();
            return Builder::EntityBuilder(id, p_ComponentManager);
        }

    private:
        std::unique_ptr<ComponentManager>& p_ComponentManager;
        Container::EntityStorage m_EntityStorage;
    };
}
