/**
 * @file entity_manager.h
 * @brief Defines the EntityManager, which handles creation and destruction of entities.
 */

#pragma once
#include <fecs/core/types.h>
#include <vector>

namespace FECS
{
    namespace Manager
    {
        /**
         * @brief Manages entity lifecycle, including creation, destruction, and version tracking.
         *
         * This class handles entity ID allocation using a freelist-based approach.
         * It also maintains versioning for safe validation of entity references.
         */
        class EntityManager
        {
        public:
            /**
             * @brief Constructs an empty EntityManager
             */
            EntityManager()
            {
            }

            /**
             * @brief Pre-allocates memory for a number of entities.
             * @param amount Number of entities to reserve space for.
             */
            void Reserve(std::uint32_t amount)
            {
                m_Versions.reserve(amount);
                m_FreeList.reserve(amount);
            }

            /**
             * @brief Creates a new Entity
             *
             * If there are free slots available from destroyed entities, they are reused.
             * Otherwise, a new index is assigned.
             *
             * @return A new valid Entity ID with embedded version.
             */
            Entity Create()
            {
                std::uint32_t idx;

                // Reuse from freelist if possible
                if (m_FreeList.empty())
                {
                    idx = static_cast<std::uint32_t>(m_Versions.size());
                    m_Versions.push_back(0); ///< First version is always 0
                }
                else
                {
                    idx = m_FreeList.back();
                    m_FreeList.pop_back();
                }
                return FECS::BuildEntityIndex(idx, m_Versions[idx]);
            }

            /**
             * @brief Destroys and entity and recycles its index
             *
             * The version at the entity's index is incremented to invalidate old references.
             *
             * @param e The entity to destroy.
             */
            void Destroy(Entity e)
            {
                std::uint32_t idx = FECS::GetEntityIndex(e);
                m_Versions[idx]++;
                m_FreeList.push_back(idx);
            }

            /**
             * @brief Checks whether an entity is still valid (alive).
             *
             * An entity is considered alive if its version matches the stored version.
             *
             * @param e The entity to check.
             * @return Returns true if the entity is alive, false otherwise
             */
            bool IsAlive(Entity e) const
            {
                std::uint32_t idx = FECS::GetEntityIndex(e);
                std::uint32_t ver = FECS::GetEntityVersion(e);
                return (idx < m_Versions.size() && m_Versions[idx] == ver);
            }

        private:
            std::vector<std::uint32_t> m_Versions; ///< Stores the version for each entity index.
            std::vector<std::uint32_t> m_FreeList; ///< Stores free indices available for reuse.
        };
    }
}
