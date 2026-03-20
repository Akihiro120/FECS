#pragma once
#include <cstdint>

namespace FECS
{
    class Entity 
    {
    public:
        Entity(std::uint32_t idx, std::uint32_t ver);
        Entity(std::uint32_t idx);
        ~Entity();

        std::uint32_t GetIndex();
        std::uint32_t GetVersion();

    private:
        std::uint32_t m_Index = 0;
    };
}
