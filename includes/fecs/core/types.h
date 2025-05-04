#pragma once
#include <cstdint>
#include <limits>

#ifndef FECS_COMPONENT_LIMIT
#define FECS_COMPONENT_LIMIT 512
#endif

#ifndef FECS_SPARSE_PAGE_SIZE
#define FECS_SPARSE_PAGE_SIZE 2048
#endif

namespace FECS
{

    using Entity = std::uint32_t;
    using ComponentIndex = std::uint32_t;

    static constexpr Entity INVALID_ENTITY = std::numeric_limits<Entity>::max();

    static constexpr std::size_t COMPONENT_LIMIT = FECS_COMPONENT_LIMIT;
    static constexpr std::size_t SPARSE_PAGE_SIZE = FECS_SPARSE_PAGE_SIZE;

    static constexpr std::uint32_t INDEX_BITS = 20;
    static constexpr std::uint32_t VERSION_BITS = 12;
    static constexpr std::uint32_t INDEX_MASK = (1u << INDEX_BITS) - 1;
    static constexpr std::uint32_t VERSION_MASK = ~INDEX_MASK;
    static constexpr std::uint32_t NPOS = std::numeric_limits<std::uint32_t>::max();

    inline Entity BuildEntityIndex(std::uint32_t index, std::uint32_t version)
    {
        return (version << INDEX_BITS) | (index & INDEX_MASK);
    }

    inline std::uint32_t GetEntityIndex(Entity e)
    {
        return e & INDEX_MASK;
    }

    inline std::uint32_t GetEntityVersion(Entity e)
    {
        return (e & VERSION_MASK) >> INDEX_BITS;
    }
}
