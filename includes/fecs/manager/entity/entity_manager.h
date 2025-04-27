#pragma once
#include <vector>
#include <fecs/core/core.h>

namespace FECS
{

    class Entity;
    class Registry;

    namespace Manager
    {
        class QueryManager;
        class ComponentManager;
    }

    namespace Manager
    {

        class EntityManager
        {
        public:
            EntityManager(Registry& registry, QueryManager& queryManager, ComponentManager& componentManager);
            ~EntityManager() = default;

            FECS::Entity CreateEntity();
            void DestroyEntity(FECS::Entity& entity);

        private:
            std::uint32_t m_NextID = 0;
            std::vector<std::uint32_t> m_FreeID;

            Registry& m_RegistryReference;
            QueryManager& m_QueryManagerReference;
            ComponentManager& m_ComponentManagerReference;
        };
    }
}
