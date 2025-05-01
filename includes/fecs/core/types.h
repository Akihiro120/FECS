#pragma once
#include <bitset>

#ifndef FECS_COMPONENT_LIMIT
#define FECS_COMPONENT_LIMIT 256
#endif

#ifndef FECS_SPARSE_MINIMUM
#define FECS_SPARSE_MINIMUM 2048
#endif

#ifndef FECS_ENTITY_MINIMUM
#define FECS_ENTITY_MINIMUM 10000
#endif

namespace FECS
{
    using SparseIndex = std::uint32_t;
    using Entity = std::size_t;
    using ComponentBit = std::size_t;

    // invalids
    constexpr std::size_t INVALID_SPARSE_INDEX = std::numeric_limits<SparseIndex>::max();
    constexpr std::size_t INVALID_ENTITY = std::numeric_limits<Entity>::max();

    // limits
    constexpr std::size_t COMPONENT_LIMIT = FECS_COMPONENT_LIMIT;
    constexpr std::size_t SPARSE_MINIMUM_SIZE = FECS_SPARSE_MINIMUM;
    constexpr std::size_t ENTITY_MINIMUM = FECS_ENTITY_MINIMUM;

    // signature
    using Signature = std::bitset<COMPONENT_LIMIT>;
    constexpr Signature EMPTY_SIGNATURE = Signature{};
}
