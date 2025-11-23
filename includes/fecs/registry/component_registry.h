#pragma once
#include "fecs/containers/sparse_set.h"
#include "fecs/manager/component_manager.h"

namespace FECS::Registries
{
    class ComponentRegistry
    {
    public:
        ComponentRegistry() = default;
        ComponentRegistry(const ComponentRegistry&) = delete;
        ComponentRegistry& operator=(const ComponentRegistry&) = delete;

        template <typename T>
        void Attach(Entity entity, const T& component)
        {
            Container::SparseSet<T>* set = m_ComponentManager.GetPool<T>();
            set->Insert(entity, component);

            // increment component version
            m_ComponentManager.GetVersion<T>()++;
        }

        template <typename T>
        void Detach(Entity entity)
        {
            Container::SparseSet<T>* set = m_ComponentManager.GetPool<T>();
            set->Remove(entity);

            m_ComponentManager.GetVersion<T>()++;
        }

        template <typename T, typename... Args>
        T& Emplace(Entity entity, Args&&... args)
        {
            Container::SparseSet<T>& set = m_ComponentManager.GetPool<T>();
            T& component = set.Emplace(entity, std::forward<Args>(args)...);

            // update version
            m_ComponentManager.GetVersion<T>()++;

            return component;
        }

        template <typename T>
        T& Get(Entity entity)
        {
            Container::SparseSet<T>* set = m_ComponentManager.GetPool<T>();
            return set->Get(entity);
        }

        template <typename T>
        bool Has(Entity entity)
        {
            Container::SparseSet<T>* set = m_ComponentManager.GetPool<T>();
            return set->Has(entity);
        }

        template <typename T>
        T* TryGet(Entity entity)
        {
            if (!Has<T>(entity))
            {
                return nullptr;
            }

            return &Get<T>(entity);
        }

    private:
        Manager::ComponentManager m_ComponentManager;
    };
}
