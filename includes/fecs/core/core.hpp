#pragma once
#include <cstdint>
#include <memory>
#include <limits>
#include <bitset>

namespace FECS
{

    template <typename T>
    using Ref = std::shared_ptr<T>;
    template <typename T>
    using OwnedRef = std::unique_ptr<T>;

    using SparseIndex = std::size_t;

    // constants
    constexpr std::uint32_t INVALID_ENTITY_INDEX = std::numeric_limits<std::uint32_t>::max();
    constexpr std::size_t INVALID_SPARSE_INDEX = std::numeric_limits<SparseIndex>::max();
    constexpr std::size_t BASE_COMPONENT_AMOUNT = 64;

    using ComponentSignature = std::bitset<BASE_COMPONENT_AMOUNT>;

}
