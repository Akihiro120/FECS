#pragma once
#include "fecs/containers/sparse_set.h"
#include "fecs/manager/component_manager.h"
#include "fecs/manager/entity_manager.h"
#include "fecs/manager/view_manager.h"

namespace FECS
{
    // special

    using namespace FECS::Manager;
    class Registry
    {
    public:
        Registry()
        {
        }

        Entity CreateEntity()
        {
            return m_EntityManager.Create();
        }

        void DestroyEntity(Entity id)
        {

            ComponentManager::DeleteEntity(id);
            ComponentManager::GetVersion<GlobalComponent>()++;

            m_EntityManager.Destroy(id);
        }

        bool IsEntityAlive(Entity id) const
        {
            return m_EntityManager.IsAlive(id);
        }

        EntityManager& GetEntityManager()
        {
            return m_EntityManager;
        }

        void Reserve(std::size_t size)
        {
            ComponentManager::Reserve(size);
        }

        template <typename T>
        Container::SparseSet<T>& RegisterComponent()
        {
            return ComponentManager::GetPool<T>(&m_EntityManager);
        }

        template <typename T>
        void Attach(Entity e, const T& component)
        {
            Container::SparseSet<T>& set = ComponentManager::GetPool<T>(&m_EntityManager);
            set.Insert(e, component);

            // rebuild flag
            ComponentManager::GetVersion<T>()++;
        }

        template <typename T>
        T& Get(Entity e)
        {
            Container::SparseSet<T>& set = ComponentManager::GetPool<T>(&m_EntityManager);
            return set.Get(e);
        }

        template <typename T>
        void Detach(Entity e)
        {
            Container::SparseSet<T>& set = ComponentManager::GetPool<T>(&m_EntityManager);
            set.Remove(e);

            // rebuild flag
            ComponentManager::GetVersion<T>()++;
        }

        template <typename T>
        void Has(Entity e)
        {
            Container::SparseSet<T>& set = ComponentManager::GetPool<T>(&m_EntityManager);
            set.Has(e);
        }

        template <typename... C>
        FECS::Manager::View<C...> View()
        {
            return FECS::Manager::View<C...>(&m_EntityManager);
        }

    private:
        Manager::EntityManager m_EntityManager;
    };
}
