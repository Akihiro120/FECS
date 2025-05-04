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
        public:
            template <typename T>
            static Container::SparseSet<T>& GetPool(EntityManager* manager)
            {
                static Container::SparseSet<T> pool;
                if (!pool.GetEntityManager())
                {
                    pool.SetEntityManager(manager);
                }
                return pool;
            }
        };
    }
}
