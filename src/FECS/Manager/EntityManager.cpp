#include "FECS/Manager/EntityManager.h"

namespace FECS::Manager
{
    EntityManager::EntityManager()
    {
    }

    auto EntityManager::Reserve(std::uint32_t amount) -> void
    {
        m_Versions.reserve(amount);
        m_FreeList.reserve(amount);
    }

    auto EntityManager::Create() -> Entity
    {
        std::uint32_t idx;

        // Reuse from freelist if possible
        if (m_FreeList.empty())
        {
            idx = static_cast<std::uint32_t>(m_Versions.size());
            m_Versions.push_back(0); ///< First version is always 0
        }
        else
        {
            idx = m_FreeList.back();
            m_FreeList.pop_back();
        }
        return FECS::BuildEntityIndex(idx, m_Versions[idx]);
    }

    auto EntityManager::Destroy(Entity e) -> void
    {
        std::uint32_t idx = FECS::GetEntityIndex(e);
        m_Versions[idx]++;
        m_FreeList.push_back(idx);
    }

    auto EntityManager::IsAlive(Entity e) const -> bool
    {
        std::uint32_t idx = FECS::GetEntityIndex(e);
        std::uint32_t ver = FECS::GetEntityVersion(e);
        return (idx < m_Versions.size() && m_Versions[idx] == ver);
    }
}