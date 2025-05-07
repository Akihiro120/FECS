/**
 * @file sparse_set.h
 * @brief Defines the SparseSet data structure for fast component storage and lookup.
 */

#pragma once
#include <fecs/core/types.h>
#include <fecs/manager/entity_manager.h>
#include <cassert>
#include <array>

namespace FECS
{
    namespace Container
    {
        /**
         * @brief Interface for type-erased sparse sets.
         *
         * Allows uniform handling of component pools without knowing their type,
         * useful in cleanup operations.
         */
        class ISparseSet
        {
        public:
            virtual ~ISparseSet() = default;

            /**
             * @brief Removes the component of a given entity.
             * @param e The entity associated with the component.
             */
            virtual void Remove(Entity e) = 0;

            /**
             * @brief Clears all components in the pool.
             */
            virtual void Clear() = 0;

            /**
             * @brief Sets the owning EntityManager for the pool.
             * @param m Pointer to the EntityManager.
             */
            virtual void SetEntityManager(Manager::EntityManager* m) = 0;

            /**
             * @brief Gets the EntityManager associated with this pool.
             * @return Pointer to the EntityManager.
             */
            virtual Manager::EntityManager* GetEntityManager() = 0;
        };

        /**
         * @brief SparseSet stores components in a densely packed array
         * while allowing fast indexed lookup via sparse indirection.
         *
         * Uses paged sparse arrays and dense vectors to provide fast
         * O(1) insertion, removal, and access with minimal memory waste.
         *
         * @tparam T The component type stored in the set.
         */
        template <typename T>
        class SparseSet : public ISparseSet
        {
        public:
            SparseSet()
            {
            }

            /**
             * @brief Inserts or overrides a component for an entity,
             * @param e The entity.
             * @param component The component data
             */
            inline void Insert(Entity e, const T& component)
            {
                assert(m_EntityManager->IsAlive(e) && "Cannot Assign to Dead Entity");
                std::uint32_t idx = FECS::GetEntityIndex(e);

                auto& slot = SparseSlot(idx);

                if (slot == NPOS)
                {
                    // new
                    slot = m_Dense.size();
                    m_DenseEntities.push_back(e);
                    m_Dense.push_back(component);
                }
                else
                {
                    // override
                    m_Dense[slot] = component;
                }
            }

            /**
             * @brief Removes the component associated with an entity.
             * @param e The entity whose component to remove.
             */
            inline virtual void Remove(Entity e) override
            {
                assert(m_EntityManager->IsAlive(e) && "Cannot Remove from Dead Entity");
                std::uint32_t idx = FECS::GetEntityIndex(e);

                auto& slot = SparseSlot(idx);
                if (slot == NPOS)
                    return;

                std::uint32_t last = m_Dense.size() - 1;
                if (slot != last)
                {
                    // Swap with last to maintain compactness
                    m_Dense[slot] = std::move(m_Dense[last]);
                    m_DenseEntities[slot] = m_DenseEntities[last];

                    SparseSlot(GetEntityIndex(m_DenseEntities[slot])) = slot;
                }

                m_Dense.pop_back();
                m_DenseEntities.pop_back();
                slot = NPOS;
            }

            /**
             * @brief Checks if the entity has a component.
             * @param e The entity.
             * @return true if the component exists
             */
            inline bool Has(Entity e) const
            {
                uint32_t idx = FECS::GetEntityIndex(e);
                if (auto* page = PageFor(idx))
                {
                    return (*page)[GetPageOffset(idx)] != NPOS;
                }

                return false;
            }

            /**
             * @brief Returns a mutable reference to the entity's components.
             * @param e The entity.
             * @return Reference to the component
             */
            inline T& Get(Entity e)
            {
                assert(m_EntityManager->IsAlive(e) && "Cannot Get on Dead Entity");
                assert(Has(e) && "Component not Present");
                std::uint32_t idx = GetEntityIndex(e);
                auto* page = PageFor(idx);
                assert(page && (*page)[GetPageOffset(idx)] != NPOS);
                return m_Dense[(*page)[GetPageOffset(idx)]];
            }

            /**
             * @brief Returns a const reference to the entity's components.
             * @param e The entity.
             * @return Reference to the component
             */
            inline const T& Get(Entity e) const
            {
                assert(m_EntityManager->IsAlive(e) && "Cannot Get on Dead Entity");
                assert(Has(e) && "Component not Present");
                std::uint32_t idx = GetEntityIndex(e);
                auto* page = PageFor(idx);
                assert(page && (*page)[GetPageOffset(idx)] != NPOS);
                return m_Dense[(*page)[GetPageOffset(idx)]];
            }

            /**
             * @brief Returns the number of components currently stored.
             * @return The size of the dense array.
             */
            inline std::size_t Size() const
            {
                return m_Dense.size();
            }

            /**
             * @brief Returns the entity as a specific index in the dense array.
             * @param i The index
             * @return The entity ID.
             */
            inline Entity EntityAt(std::uint32_t i) const
            {
                return m_DenseEntities[i];
            }

            /**
             * @brief Pre-allocates memory for the given number of components.
             * @param amount Number of component entries to reserve.
             */
            inline void Reserve(std::size_t amount)
            {
                std::uint32_t numPages = (amount + SPARSE_PAGE_SIZE - 1) / SPARSE_PAGE_SIZE;

                if (m_Sparse.size() < numPages)
                {
                    m_Sparse.resize(numPages, nullptr);
                }

                for (std::size_t p = 0; p < numPages; ++p)
                {
                    if (!m_Sparse[p])
                    {
                        m_Sparse[p] = new std::array<std::uint32_t, SPARSE_PAGE_SIZE>();
                        m_Sparse[p]->fill(NPOS);
                    }
                }

                m_Dense.reserve(amount);
                m_DenseEntities.reserve(amount);
            }

            /**
             * @brief Assigns the EntityManager used for liveness checks.
             * @param m Pointer to the EntityManager.
             */
            virtual void SetEntityManager(Manager::EntityManager* m) override
            {
                m_EntityManager = m;
            }

            /**
             * @brief Returns the EntityManager associated with this pool.
             * @return Pointer to the EntityManager.
             */
            virtual Manager::EntityManager* GetEntityManager() override
            {
                return m_EntityManager;
            }

            /**
             * @brief Clears the sparse and dense storage completely.
             */
            inline virtual void Clear() override
            {
                for (auto page : m_Sparse)
                {
                    if (page)
                        page->fill(NPOS);
                }
                m_Dense.clear();
                m_DenseEntities.clear();
            }

        private:
            /// Returns a mutable reference to the sparse slot for the given index, allocating if needed.
            std::uint32_t& SparseSlot(std::uint32_t idx)
            {
                std::uint32_t p = GetPageIndex(idx);
                if (p >= m_Sparse.size())
                {
                    m_Sparse.resize(p + 1, nullptr);
                }

                if (!m_Sparse[p])
                {
                    m_Sparse[p] = new std::array<std::uint32_t, SPARSE_PAGE_SIZE>();
                    m_Sparse[p]->fill(NPOS);
                }

                return (*m_Sparse[p])[GetPageOffset(idx)];
            }

            /// Gets the page index from a global index.
            inline const std::uint32_t GetPageIndex(std::uint32_t idx) const
            {
                return idx / SPARSE_PAGE_SIZE;
            }

            /// Gets the offset within a sparse page.
            inline const std::uint32_t GetPageOffset(std::uint32_t idx) const
            {
                return idx % SPARSE_PAGE_SIZE;
            }

            /// Returns the page pointer for a given index.
            std::array<std::uint32_t, SPARSE_PAGE_SIZE>* PageFor(std::uint32_t idx)
            {
                std::uint32_t p = GetPageIndex(idx);
                if (p >= m_Sparse.size())
                    return nullptr;
                return m_Sparse[p];
            }

            /// Returns the const page pointer for a given index.
            const std::array<std::uint32_t, SPARSE_PAGE_SIZE>* PageFor(std::uint32_t idx) const
            {
                std::uint32_t p = GetPageIndex(idx);
                if (p >= m_Sparse.size())
                    return nullptr;
                return m_Sparse[p];
            }

            std::vector<T> m_Dense;                                             ///< Densely packed component values.
            std::vector<Entity> m_DenseEntities;                                ///< Entity IDs parallel to m_Dense.
            std::vector<std::array<std::uint32_t, SPARSE_PAGE_SIZE>*> m_Sparse; ///< Page sparse array of indices.
            Manager::EntityManager* m_EntityManager;                            ///< Pointer to the entity manager
        };
    }
}
