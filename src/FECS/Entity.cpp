#include "Entity.h"
// #include <iostream>

namespace FECS
{
    static constexpr std::uint32_t INDEX_BITS = 20;
    // static constexpr std::uint32_t VERSION_BITS = 12;
    static constexpr std::uint32_t INDEX_MASK = (1u << INDEX_BITS) - 1;
    static constexpr std::uint32_t VERSION_MASK = ~INDEX_MASK;

    Entity::Entity(std::uint32_t idx, std::uint32_t ver)
    {
        this->m_Index = (ver << INDEX_BITS) | (idx & INDEX_MASK);
    }

    Entity::Entity(std::uint32_t idx)
    {
        this->m_Index = idx;
    }

    Entity::~Entity()
    {
        // std::cout << "Deconstructed Entity Index: " << GetIndex() << " Generation: " << GetVersion() << std::endl;
    }

    std::uint32_t Entity::GetIndex()
    {
        return m_Index & INDEX_MASK;
    }

    std::uint32_t Entity::GetVersion()
    {
        return (m_Index & VERSION_MASK) >> INDEX_BITS;
    }
}
