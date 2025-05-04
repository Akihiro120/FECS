#pragma once
#include <fecs/core/types.h>
#include <vector>

namespace FECS
{
    namespace Manager
    {
        class EntityManager
        {
        public:
            EntityManager()
            {
            }

            void Reserve(std::uint32_t amount)
            {
                m_Versions.reserve(amount);
                m_FreeList.reserve(amount);
            }

            Entity Create()
            {
                std::uint32_t idx;
                if (m_FreeList.empty())
                {
                    idx = static_cast<std::uint32_t>(m_Versions.size());
                    m_Versions.push_back(0);
                }
                else
                {
                    idx = m_FreeList.back();
                    m_FreeList.pop_back();
                }
                return FECS::BuildEntityIndex(idx, m_Versions[idx]);
            }

            void Destroy(Entity e)
            {
                std::uint32_t idx = FECS::GetEntityIndex(e);
                m_Versions[idx]++;
                m_FreeList.push_back(idx);
            }

            bool IsAlive(Entity e) const
            {
                std::uint32_t idx = FECS::GetEntityIndex(e);
                std::uint32_t ver = FECS::GetEntityVersion(e);
                return (idx < m_Versions.size() && m_Versions[idx] == ver);
            }

        private:
            std::vector<std::uint32_t> m_Versions; // generation per entity
            std::vector<std::uint32_t> m_FreeList; // recycled entities
        };
    }
}
