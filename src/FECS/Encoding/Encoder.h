#pragma once
#include "ICommand.h"
#include <vector>

namespace FECS
{
    class Encoder
    {
    public:
        Encoder();
        ~Encoder();


    private:
        std::vector<ICommand*> m_Commands;

        // friend specifier for FECS::World
        friend class World;
    };
}
