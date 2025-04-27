#pragma once
#include <cstdint>
#include <limits>

namespace FECS
{

    using SparseIndex = std::size_t;

    // constants
    constexpr std::uint32_t INVALID_ENTITY_INDEX = std::numeric_limits<std::uint32_t>::max();
    constexpr std::size_t INVALID_SPARSE_INDEX = std::numeric_limits<SparseIndex>::max();

    constexpr std::size_t MAX_COMPONENTS = 512;
}
