#pragma once
#include "FECS/Containers/ComponentStorage.h"
#include "FECS/Containers/SparseSet.h"

namespace FECS::Manager
{
    using namespace Container;

    class ComponentManager
    {
    public:
        ComponentManager() = default;

        template <typename T>
        auto Attach(Entity entity, const T& component) -> void
        {
            SparseSet<T>* pool = m_ComponentStorage.GetPool<T>();
            pool->Insert(entity, component);
        }

        template <typename T>
        auto Attach(Entity entity, T&& component) -> void
        {
            SparseSet<T>* pool = m_ComponentStorage.GetPool<T>();
            pool->Insert(entity, std::move(component));
        }

        template <typename T, typename... Args>
        auto Emplace(Entity entity, Args&&... args) -> void
        {
            SparseSet<T>* pool = m_ComponentStorage.GetPool<T>();
            pool->Emplace(entity, std::forward<Args>(args)...);
        }

        template <typename T>
        auto Detach(Entity entity) -> void
        {
            SparseSet<T>* pool = m_ComponentStorage.GetPool<T>();
            pool->Remove(entity);
        }

        template <typename T>
        auto Get(Entity entity) -> T&
        {
            SparseSet<T>* pool = m_ComponentStorage.GetPool<T>();
            return pool->Get(entity);
        }

        template <typename T>
        auto Has(Entity entity) -> bool
        {
            SparseSet<T>* pool = m_ComponentStorage.GetPool<T>();
            return pool->Has(entity);
        }

        auto DetachAllFromEntity(Entity entity) -> void
        {
            m_ComponentStorage.DeleteEntity(entity);
        }

    private:
        Container::ComponentStorage m_ComponentStorage;
    };
}
