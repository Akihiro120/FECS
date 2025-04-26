#pragma once
#include <vector>
#include <fecs/core/core.hpp>

namespace FECS
{

    class Entity;
    class Registry;

    namespace Manager
    {

        class EntityManager
        {
        public:
            EntityManager(Registry& registry);
            /**
 * @brief Destroys the EntityManager instance.
 *
 * Releases resources held by the EntityManager. Does not affect the associated Registry or managed entities.
 */
~EntityManager() = default;

            FECS::Entity CreateEntity();
            void DestroyEntity(FECS::Entity& entity);

        private:
            std::uint32_t m_NextID = 0;
            std::vector<std::uint32_t> m_FreeID;

            Registry& m_RegistryReference;
        };
    }
}
