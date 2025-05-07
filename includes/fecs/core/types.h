/**
 * @file types.h
 * @brief Defines core types and constants for the FECS ECS system.
 *
 * Includes definitions for entities, component indexing, sparse page sizing,
 * and utility functions for encoding and decoding entity IDs.
 */

#pragma once
#include <cstdint>
#include <limits>

/// @brief Defines the default number of elements per sparse page, if not specified
#ifndef FECS_SPARSE_PAGE_SIZE
#define FECS_SPARSE_PAGE_SIZE 2048
#endif

namespace FECS
{
    /**
     * @brief Dummy tag component used for tracking global ECS changes (e.g. full view rebuilds).
     */
    struct GlobalComponent
    {
    };

    /// @brief Type alias for entity IDs (32-bit unsigned integer)
    using Entity = std::uint32_t;

    /// @brief Type alias for identifying component type indices.
    using ComponentIndex = std::uint32_t;

    /// @brief Presents an invalid entity ID.
    static constexpr Entity INVALID_ENTITY = std::numeric_limits<Entity>::max();

    /// @brief Defines the number of entities per sparse set page.
    static constexpr std::size_t SPARSE_PAGE_SIZE = FECS_SPARSE_PAGE_SIZE;

    /// @brief Number of bits used to store the entity index (lower 20 bits).
    static constexpr std::uint32_t INDEX_BITS = 20;

    /// @brief Number of bits used to store the entity version (upper 12 bits).
    static constexpr std::uint32_t VERSION_BITS = 12;

    /// @brief Bitmask to extract the entity index portion.
    static constexpr std::uint32_t INDEX_MASK = (1u << INDEX_BITS) - 1;

    /// @brief Bitmask to extract the entity version portion.
    static constexpr std::uint32_t VERSION_MASK = ~INDEX_MASK;

    /// @brief Special constant used to indicate a "not found" index.
    static constexpr std::uint32_t NPOS = std::numeric_limits<std::uint32_t>::max();

    /**
     * @brief Combines an entity index and version into a single 32-bit entity ID.
     *
     * The lower 20 bits are used for the index, and the upper 12 for the version.
     *
     * @param index The entity index.
     * @param version The entity verison.
     * @return A composed 32-bit entity ID.
     */
    inline Entity BuildEntityIndex(std::uint32_t index, std::uint32_t version)
    {
        return (version << INDEX_BITS) | (index & INDEX_MASK);
    }

    /**
     * @brief Extracts the index portion from an entity ID.
     * @param e The encoded entity ID.
     * @return The raw entity index.
     */
    inline std::uint32_t GetEntityIndex(Entity e)
    {
        return e & INDEX_MASK;
    }

    /**
     * @brief Extracts the verison portion from an entity ID.
     * @param e The encoded entity ID.
     * @return The version component of the entity.
     */
    inline std::uint32_t GetEntityVersion(Entity e)
    {
        return (e & VERSION_MASK) >> INDEX_BITS;
    }
}
