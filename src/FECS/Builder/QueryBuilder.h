#pragma once
#include <array>
#include <tuple>
#include "FECS/Containers/SparseSet.h"
#include "FECS/Core/Types.h"
#include "FECS/Manager/ComponentManager.h"

namespace FECS::Builder
{
    using namespace Manager;

    template <typename... Components>
    class QueryBuilder
    {
    private:
        struct PoolInfo
        {
            size_t size;
            fast_vector<Entity>* entities;
        };

    public:
        QueryBuilder(ComponentManager* manager)
            : p_ComponentManager(manager),
              m_Pools(std::make_tuple(manager->GetStorages().template GetPool<Components>()...))
        {
        }
        ~QueryBuilder() = default;

        template <typename Func>
        auto Each(Func&& queryFunction) -> void
        {
            std::array<PoolInfo, sizeof...(Components)> info = {
                PoolInfo{
                    std::get<SparseSet<Components>*>(m_Pools)->Size(),
                    &std::get<SparseSet<Components>*>(m_Pools)->GetEntities()}...};

            const PoolInfo* smallest = &info[0];
            for (const auto& item : info)
            {
                if (item.size < smallest->size)
                {
                    smallest = &item;
                }
            }

            for (Entity e : *smallest->entities)
            {
                if ((std::get<SparseSet<Components>*>(m_Pools)->Has(e) && ...))
                {
                    queryFunction(e, std::get<SparseSet<Components>*>(m_Pools)->Get(e)...);
                }
            }
        }

    private:
        using PoolTuple = std::tuple<SparseSet<Components>*...>;

        PoolTuple m_Pools;
        ComponentManager* p_ComponentManager;
    };
}
