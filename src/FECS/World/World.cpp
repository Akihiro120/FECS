#include "World.h"
#include "FECS/Encoding/Encoder.h"
#include "FECS/Entity.h"

namespace FECS
{
    World::World()
    {

    }

    World::~World()
    {

    }

    Entity World::SpawnEntity()
    {

    }

    void World::Finish(Encoder&& enc)
    {
        // execute the commands
        for (ICommand* cmd : enc.m_Commands)
        {
            cmd->Execute(*this);
            delete cmd;
        }
        enc.m_Commands.clear();
    }

    Encoder World::CreateEncoder()
    {
        return Encoder();
    }
}
