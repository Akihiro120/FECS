#pragma once
#include <cassert>
#include <array>
#include <vector>

#include "FECS/Core/Types.h"
#include "FECS/Containers/ISparseSet.h"

namespace FECS::Container
{
    template <typename T>
    class SparseSet : public ISparseSet
    {
    public:
        SparseSet()
        {
        }

        ~SparseSet()
        {
            for (auto* page : m_Sparse)
            {
                delete page;
            }
            m_Sparse.clear();
        }

        inline void Insert(Entity e, const T& component)
        {
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

        inline void Insert(Entity e, T&& component)
        {
            std::uint32_t idx = FECS::GetEntityIndex(e);
            auto& slot = SparseSlot(idx);

            if (slot == NPOS)
            {
                slot = m_Dense.size();
                m_DenseEntities.push_back(e);
                m_Dense.push_back(std::move(component));
            }
            else
            {
                m_Dense[slot] = std::move(component);
            }
        }

        template <typename... Args>
        inline T& Emplace(Entity e, Args&... args)
        {
            std::uint32_t idx = FECS::GetEntityIndex(e);
            auto& slot = SparseSlot(idx);

            if (slot == NPOS)
            {
                slot = m_Dense.size();
                m_DenseEntities.push_back(std::move(e));
                m_Dense.emplace_back(std::forward<Args>(args)...);
                return m_Dense[slot];
            }
            else
            {
                m_Dense[slot] = T(std::forward<Args>(args)...);
                return m_Dense[slot];
            }
        }

        inline virtual void Remove(Entity e) override
        {
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

        inline bool Has(Entity e)
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
            assert(Has(e));
            std::uint32_t idx = GetEntityIndex(e);
            auto* page = PageFor(idx);
            assert(page && (*page)[GetPageOffset(idx)] != NPOS);
            return m_Dense[(*page)[GetPageOffset(idx)]];
        }

        inline const T& Get(Entity e) const
        {
            assert(Has(e));
            std::uint32_t idx = GetEntityIndex(e);
            auto* page = PageFor(idx);
            assert(page && (*page)[GetPageOffset(idx)] != NPOS);
            return m_Dense[(*page)[GetPageOffset(idx)]];
        }

        inline std::size_t Size() const
        {
            return m_Dense.size();
        }

        inline Entity EntityAt(std::uint32_t i)
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

        inline auto GetEntities() -> std::vector<Entity>&
        {
            return m_DenseEntities;
        }

        // Expose raw data for optimization
        inline auto GetDataVector() -> std::vector<T>&
        {
            return m_Dense;
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

        std::vector<T> m_Dense;
        std::vector<Entity> m_DenseEntities;
        std::vector<std::array<std::uint32_t, SPARSE_PAGE_SIZE>*> m_Sparse;
    };
}
