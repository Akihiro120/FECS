#pragma once
#include <iostream>

namespace FECS
{
    class World;

    struct ICommand
    {
        virtual void Execute(World& world) = 0;
        virtual ~ICommand()
        {
            std::cout << "Consumed Command" << std::endl;
        };
    };
}
