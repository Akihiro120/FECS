#pragma once
#include <fecs/core/types.h>
#include <vector>

namespace FECS::Manager
{
    class EntityManager
    {
    public:
        EntityManager();

        auto Reserve(std::uint32_t amount) -> void;

        auto Create() -> Entity;

        auto Destroy(Entity e) -> void;

        auto IsAlive(Entity e) const -> bool;

    private:
        std::vector<std::uint32_t> m_Versions;
        std::vector<std::uint32_t> m_FreeList;
    };
}
