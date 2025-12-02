#pragma once
#include "FECS/Containers/ComponentStorage.h"
#include "FECS/Containers/SparseSet.h"

namespace FECS::Manager
{
    class ComponentManager
    {
    public:
        ComponentManager() = default;

        template <typename T>
        auto Attach(Entity entity, const T& component) -> void
        {
            Container::SparseSet<T>* pool = m_ComponentStorage.GetPool<T>();
            pool->Insert(entity, component);
        }

        template <typename T>
        auto Attach(Entity entity, T&& component) -> void
        {
            Container::SparseSet<T>* pool = m_ComponentStorage.GetPool<T>();
            pool->Insert(entity, std::move(component));
        }

        template <typename T, typename... Args>
        auto Emplace(Entity entity, Args&&... args) -> void
        {
            Container::SparseSet<T>* pool = m_ComponentStorage.GetPool<T>();
            pool->Emplace(entity, std::forward<Args>(args)...);
        }

        template <typename T>
        auto Detach(Entity entity) -> void
        {
            Container::SparseSet<T>* pool = m_ComponentStorage.GetPool<T>();
            pool->Remove(entity);
        }

        template <typename T>
        auto Get(Entity entity) -> T&
        {
            Container::SparseSet<T>* pool = m_ComponentStorage.GetPool<T>();
            return pool->Get(entity);
        }

        template <typename T>
        auto Has(Entity entity) -> bool
        {
            Container::SparseSet<T>* pool = m_ComponentStorage.GetPool<T>();
            return pool->Has(entity);
        }

        template <typename T>
        auto Reserve(size_t capacity) -> void
        {
            Container::SparseSet<T>* pool = m_ComponentStorage.GetPool<T>();
            pool->Reserve(capacity);
        }

        auto DetachAllFromEntity(Entity entity) -> void
        {
            m_ComponentStorage.DeleteEntity(entity);
        }

        auto GetStorages() -> Container::ComponentStorage&
        {
            return m_ComponentStorage;
        }

    private:
        Container::ComponentStorage m_ComponentStorage;
    };
}
