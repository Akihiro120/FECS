#pragma once
#include "fecs/core/Types.h"

namespace FECS::Container
{
    /**
     * @brief Interface for type-erased sparse sets.
     *
     * Allows uniform handling of component pools without knowing their type,
     * useful in cleanup operations.
     */
    class ISparseSet
    {
    public:
        virtual ~ISparseSet() = default;

        /**
         * @brief Removes the component of a given entity.
         * @param e The entity associated with the component.
         */
        virtual void Remove(Entity e) = 0;

        /**
         * @brief Clears all components in the pool.
         */
        virtual void Clear() = 0;
    };
}
