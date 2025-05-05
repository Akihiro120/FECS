#pragma once
#include <iostream>
#include <fecs/core/types.h>
#include <fecs/manager/entity_manager.h>
#include <cassert>
#include <array>

namespace FECS
{
    namespace Container
    {
        class ISparseSet
        {
        public:
            virtual ~ISparseSet() = default;
            virtual void Remove(Entity e) = 0;
        };

        template <typename T>
        class SparseSet : public ISparseSet
        {
        public:
            SparseSet()
            {
            }

            inline void Insert(Entity e, const T& component)
            {
                assert(m_EntityManager->IsAlive(e) && "Cannot Assign to Dead Entity");
                std::uint32_t idx = FECS::GetEntityIndex(e);

                auto& slot = SparseSlot(idx);

                if (slot == NPOS)
                {
                    slot = m_Dense.size();
                    m_DenseEntities.push_back(e);
                    m_Dense.push_back(component);
                }
                else
                {
                    m_Dense[slot] = component;
                }
            }

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
                    m_Dense[slot] = std::move(m_Dense[last]);
                    m_DenseEntities[slot] = m_DenseEntities[last];

                    SparseSlot(GetEntityIndex(m_DenseEntities[slot])) = slot;
                }

                m_Dense.pop_back();
                m_DenseEntities.pop_back();
                slot = NPOS;
            }

            inline bool Has(Entity e) const
            {
                uint32_t idx = FECS::GetEntityIndex(e);
                if (auto* page = PageFor(idx))
                {
                    return (*page)[GetPageOffset(idx)] != NPOS;
                }

                return false;
            }

            inline T& Get(Entity e)
            {
                assert(Has(e) && "Component not Present");
                std::uint32_t idx = GetEntityIndex(e);
                assert(m_EntityManager->IsAlive(e) && "Cannot Get from Dead Entity");
                auto* page = PageFor(idx);
                assert(page && (*page)[GetPageOffset(idx)] != NPOS);
                return m_Dense[(*page)[GetPageOffset(idx)]];
            }

            inline const T& Get(Entity e) const
            {
                assert(Has(e) && "Component not Present");
                std::uint32_t idx = GetEntityIndex(e);
                assert(m_EntityManager->IsAlive(e) && "Cannot Get from Dead Entity");
                auto* page = PageFor(idx);
                assert(page && (*page)[GetPageOffset(idx)] != NPOS);
                return m_Dense[(*page)[GetPageOffset(idx)]];
            }

            inline std::size_t Size() const
            {
                return m_Dense.size();
            }

            inline Entity EntityAt(std::uint32_t i) const
            {
                return m_DenseEntities[i];
            }

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

            void SetEntityManager(Manager::EntityManager* m)
            {
                m_EntityManager = m;
            }

            Manager::EntityManager* GetEntityManager()
            {
                return m_EntityManager;
            }

        private:
            std::uint32_t& SparseSlot(std::uint32_t idx)
            {
                std::uint32_t p = GetPageIndex(idx);
                if (p >= m_Sparse.size())
                {
                    m_Sparse.resize(p + 1, nullptr);
                }

                if (!m_Sparse[p])
                {
                    std::cout << "Resize" << std::endl;
                    m_Sparse[p] = new std::array<std::uint32_t, SPARSE_PAGE_SIZE>();
                    m_Sparse[p]->fill(NPOS);
                }

                return (*m_Sparse[p])[GetPageOffset(idx)];
            }

            inline const std::uint32_t GetPageIndex(std::uint32_t idx) const
            {
                return idx / SPARSE_PAGE_SIZE;
            }

            inline const std::uint32_t GetPageOffset(std::uint32_t idx) const
            {
                return idx % SPARSE_PAGE_SIZE;
            }

            std::array<std::uint32_t, SPARSE_PAGE_SIZE>* PageFor(std::uint32_t idx)
            {
                std::uint32_t p = GetPageIndex(idx);
                if (p >= m_Sparse.size())
                    return nullptr;
                return m_Sparse[p];
            }

            const std::array<std::uint32_t, SPARSE_PAGE_SIZE>* PageFor(std::uint32_t idx) const
            {
                std::uint32_t p = GetPageIndex(idx);
                if (p >= m_Sparse.size())
                    return nullptr;
                return m_Sparse[p];
            }

            std::vector<T> m_Dense;
            std::vector<Entity> m_DenseEntities;
            std::vector<std::array<std::uint32_t, SPARSE_PAGE_SIZE>*> m_Sparse;
            Manager::EntityManager* m_EntityManager;
        };
    }
}
