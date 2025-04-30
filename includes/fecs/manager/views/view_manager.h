#pragma once
#include <fecs/manager/component/component_manager.h>

namespace FECS
{
    namespace Manager
    {
        template <typename... Components>
        class View
        {
        public:
            View(ComponentManager* manager) : m_Manager(manager)
            {
                m_ComponentBits = {ComponentTypeIndex::GetBitIndex<Components>()...};

                for (size_t i = 0; i < sizeof...(Components); i++)
                {
                    ComponentBit bit = m_ComponentBits[i];
                    m_LastGenerations[i] = bit < manager->GetComponentGenerations().size() ? manager->GetComponentGenerations()[bit] : 0;
                }

                Rebuild();
            }

            bool NeedsRebuild() const
            {
                for (std::size_t i = 0; i < sizeof...(Components); i++)
                {
                    ComponentBit bit = m_ComponentBits[i];
                    if (bit >= m_Manager->GetComponentGenerations().size())
                    {
                        continue;
                    }

                    if (m_Manager->GetComponentGenerations()[bit] > m_LastGenerations[i])
                    {
                        return true;
                    }
                }

                return false;
            }

            void Rebuild()
            {
                m_Entities.clear();

                Signature mask;
                for (std::size_t i = 0; i < sizeof...(Components); i++)
                {
                    mask.set(m_ComponentBits[i]);
                }

                for (auto& [signature, entities] : m_Manager->GetQueries())
                {
                    auto list = entities.GetAll();
                    if ((signature & mask) == mask)
                    {
                        m_Entities.insert(list.end(), list.begin(), list.end());
                    }
                }

                for (std::size_t i = 0; i < sizeof...(Components); i++)
                {
                    ComponentBit bit = m_ComponentBits[i];
                    if (bit < m_Manager->GetComponentGenerations().size())
                    {
                        m_LastGenerations[i] = m_Manager->GetComponentGenerations()[bit];
                    }
                }
            }

            template <typename Func>
            void Each(Func&& func)
            {
                if (NeedsRebuild())
                {
                    Rebuild();
                }

                for (Entity e : m_Entities)
                {
                    func(e, *m_Manager->GetComponentPool<Components>()->Get(e)...);
                }
            }

            // Access to raw entity list
            const std::vector<Entity>& Entities() const
            {
                return m_Entities;
            }

            // Size helper
            size_t Size() const
            {
                return m_Entities.size();
            }

            // Iterators for range-based for loops
            auto begin()
            {
                if (NeedsRebuild())
                    Rebuild();
                return m_Entities.begin();
            }

            auto end()
            {
                return m_Entities.end();
            }

            // Component access helper
            template <typename T>
            T* Get(Entity entity) const
            {
                return m_Manager->GetComponentPool<T>()->Get(entity);
            }

        private:
            ComponentManager* m_Manager;
            std::vector<Entity> m_Entities;
            std::array<uint64_t, sizeof...(Components)> m_LastGenerations;
            std::array<ComponentBit, sizeof...(Components)> m_ComponentBits;
        };
    }
}
