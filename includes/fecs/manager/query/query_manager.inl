#pragma once
#include <fecs/manager/query/query_manager.h>
#include <fecs/manager/component/component_manager.h>

namespace FECS
{

    namespace Manager
    {
        template <typename... Components, typename Function>
        inline void QueryManager::Each(Function&& queryFunction)
        {
            // quick check to see if the needed components arent registered
            (CreateComponentSignature(typeid(Components)), ...);

            Signature requiredBits;
            (requiredBits.set(m_ComponentBit[typeid(Components)]), ...);

            // get entities
            for (auto& [signature, set] : m_Queries)
            {
                if ((signature & requiredBits) == requiredBits)
                {
                    for (auto id : set.GetAll())
                    {
                        queryFunction(id, m_ComponentManagerReference.Get<Components>(id)...);
                    }
                }
            }
        }
    }
}
