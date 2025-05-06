#pragma once
#include <fecs/core/types.h>
#include "fecs/containers/sparse_set.h"
#include "fecs/manager/entity_manager.h"

namespace FECS
{
    namespace Manager
    {
        class ComponentManager
        {
        private:
            inline static std::vector<Container::ISparseSet*> m_RegisteredComponents;

            ComponentManager() = default;

        public:
            template <typename T>
            static Container::SparseSet<T>&
            GetPool(EntityManager* manager)
            {
                static Container::SparseSet<T> pool;
                if (!pool.GetEntityManager())
                {
                    // register it
                    m_RegisteredComponents.push_back(&pool);
                    pool.SetEntityManager(manager);
                }
                return pool;
            }

            template <typename T>
            static std::uint32_t& GetVersion()
            {
                static std::uint32_t version;
                return version;
            }

            // reserve a predicted amount of components
            static void Reserve(std::size_t size)
            {
                m_RegisteredComponents.reserve(size);
            }

            static void DeleteEntity(Entity e)
            {
                for (auto& comps : m_RegisteredComponents)
                {
                    comps->Remove(e);
                }
            }

            static void ClearRegistry()
            {
                for (auto& comps : m_RegisteredComponents)
                {
                    comps->Clear();
                    GetVersion<GlobalComponent>()++;
                }
            }
        };
    }
}
