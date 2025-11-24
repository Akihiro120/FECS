#pragma once
#include "FECS/Containers/ComponentStorage.h"
#include "FECS/Containers/SparseSet.h"

namespace FECS::Manager
{
    using namespace Container;

    class ComponentManager
    {
    public:
        ComponentManager();

        template <typename T>
        void Attach(Entity entity, const T& component)
        {
            SparseSet<T>* pool = m_ComponentStorage.GetPool<T>();
            pool->Insert(entity, component);
        }

    private:
        Container::ComponentStorage m_ComponentStorage;
    };
}
