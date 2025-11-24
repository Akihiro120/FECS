#pragma once
#include "FECS/Core/Types.h"
#include "FECS/Manager/ComponentManager.h"
#include <functional>
#include <memory>

namespace FECS::Builder
{
    using namespace Manager;

    class EntityBuilder
    {
    public:
        EntityBuilder(Entity entity, std::unique_ptr<ComponentManager>& manager)
            : p_ComponentManager(manager)
        {
            m_ModifiedEntity = entity;
        }

        inline auto Build() -> Entity
        {
            return m_ModifiedEntity;
        }

        template <typename T>
        inline auto Attach(const T& component) -> EntityBuilder&
        {
            p_ComponentManager->Attach<T>(m_ModifiedEntity, component);
            return *this;
        }

        template <typename T>
        inline auto Attach(T&& component) -> EntityBuilder&
        {
            p_ComponentManager->Attach<T>(m_ModifiedEntity, std::move(component));
            return *this;
        }

        template <typename T, typename... Args>
        inline auto Emplace(Args&&... args) -> EntityBuilder&
        {
            p_ComponentManager->Emplace<T>(m_ModifiedEntity, std::forward<Args>(args)...);
            return *this;
        }

        template <typename T>
        inline auto Tag(const T& component) -> EntityBuilder&
        {
            p_ComponentManager->Attach<T>(m_ModifiedEntity, T{});
            return *this;
        }

        template <typename T>
        inline auto Detach() -> EntityBuilder&
        {
            p_ComponentManager->Detach<T>(m_ModifiedEntity);
            return *this;
        }

        inline auto Apply(std::function<void(EntityBuilder&)> func) -> EntityBuilder&
        {
            func(*this);
            return *this;
        }

        template <typename T>
        inline auto Patch(std::function<void(T&)> func) -> EntityBuilder&
        {
            T& component = p_ComponentManager->Get<T>(m_ModifiedEntity);
            func(component);
            return *this;
        }

    private:
        Entity m_ModifiedEntity = INVALID_ENTITY;
        std::unique_ptr<ComponentManager>& p_ComponentManager;
    };
}
