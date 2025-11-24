#pragma once
#include "FECS/Core/Types.h"
#include "FECS/Manager/ComponentManager.h"
#include <memory>

namespace FECS::Builder
{
    using namespace Manager;

    class EntityBuilder
    {
    public:
        EntityBuilder(Entity entity, std::shared_ptr<ComponentManager> manager)
            : p_ComponentManager(manager)
        {
            m_ModifiedEntity = entity;
        }

        auto Build() -> Entity;

        template <typename T>
        auto Attach(const T& component) -> EntityBuilder&
        {
            p_ComponentManager->Attach<T>(m_ModifiedEntity, component);
        }

        auto Detach() -> EntityBuilder&
        {
        }

    private:
        Entity m_ModifiedEntity = INVALID_ENTITY;
        std::shared_ptr<ComponentManager> p_ComponentManager;
    };
}
