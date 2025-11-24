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
        void Attach(Entity entity, const T& component)
        {
            SparseSet<T>* pool = m_ComponentStorage.GetPool<T>();
            pool->Insert(entity, component);
        }

        template <typename T>
        void Attach(Entity entity, T&& component)
        {
            SparseSet<T>* pool = m_ComponentStorage.GetPool<T>();
            pool->Insert(entity, std::move(component));
        }

        template <typename T, typename... Args>
        void Emplace(Entity entity, Args&&... args)
        {
            SparseSet<T>* pool = m_ComponentStorage.GetPool<T>();
            pool->Emplace(entity, std::forward<Args>(args)...);
        }

        template <typename T>
        T& Get(Entity entity)
        {
            SparseSet<T>* pool = m_ComponentStorage.GetPool<T>();
            return pool->Get(entity);
        }

    private:
        Container::ComponentStorage m_ComponentStorage;
    };
}
