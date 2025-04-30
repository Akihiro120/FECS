#pragma once
#include <fecs/core/types.h>
#include <vector>
#include <stdexcept>

namespace FECS
{
    namespace Manager
    {
        class EntityManager
        {
        public:
            EntityManager()
            {
                m_FreeIDs.reserve(ENTITY_MINIMUM);
            }
            ~EntityManager() = default;

            Entity CreateEntity()
            {
                if (!m_FreeIDs.empty())
                {
                    Entity id = INVALID_ENTITY;
                    id = m_FreeIDs.back();
                    m_FreeIDs.pop_back();
                    return id;
                }

                Entity id = m_NextID;
                m_NextID++;
                return id;
            }

            void DestroyEntity(Entity& entity)
            {
                if (entity == INVALID_ENTITY)
                {
                    throw std::runtime_error("FECS::Entity::Remove: Cannot Remove Invalid Entity");
                }

                m_FreeIDs.emplace_back(entity);
                entity = INVALID_ENTITY;
            }

            std::size_t Size()
            {
                return m_NextID - m_FreeIDs.size();
            }

        private:
            std::vector<Entity> m_FreeIDs;
            Entity m_NextID = 0;
        };
    }
}
