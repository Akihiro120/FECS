#pragma once
#include <cstdint>

namespace FECS
{
    class Entity 
    {
    public:
        Entity(std::uint32_t idx, std::uint32_t ver);
        ~Entity();

    private:
        std::uint32_t m_Index = 0;
    };
}
