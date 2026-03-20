#pragma once
#include <iostream>
#include "FECS/Encoding/Encoder.h"
#include "FECS/Entity.h"
#include "FECS/Entity/EntityAllocator.h"

namespace FECS
{
    class World
    {
    public:
        World();
        ~World();

        Encoder CreateEncoder();
        void Finish(Encoder&& enc);
        Entity SpawnEntity();

    private:
        EntityAllocator m_EntityAllocator;
    };
}
