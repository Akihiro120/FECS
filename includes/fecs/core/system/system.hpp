#pragma once

namespace FECS::Core
{

    class System
    {
    public:
        virtual ~System() = default;
        virtual void update() = 0;
    };

}
