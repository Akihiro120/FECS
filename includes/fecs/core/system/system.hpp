#pragma once

namespace FECS::Core
{

    class System
    {
    public:
        /**
 * @brief Virtual destructor for the System base class.
 *
 * Ensures proper cleanup of resources in derived classes when deleted through a base class pointer.
 */
virtual ~System() = default;
        virtual void update() = 0;
    };

}
