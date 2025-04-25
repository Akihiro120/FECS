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

    using EntityID = std::uint32_t;
    using SparseIndex = std::size_t;

    // constants
    constexpr std::size_t INVALID_SPARSE_INDEX = std::numeric_limits<SparseIndex>::max();
    constexpr EntityID INVALID_ENTITY_ID = std::numeric_limits<EntityID>::max();
    constexpr EntityID BASE_COMPONENT_AMOUNT = 64;

    using ComponentSignature = std::bitset<BASE_COMPONENT_AMOUNT>;

} 
