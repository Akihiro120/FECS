#pragma once
#include <algorithm>
#include <cassert>
#include <array>
#include <vector>

#include "FECS/Containers/fast_vector.h"
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

        inline auto Insert(Entity e, const T& component) -> void
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

        inline auto Insert(Entity e, T&& component) -> void
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
        inline auto Emplace(Entity e, Args&... args) -> T&
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

        inline virtual auto Remove(Entity e) -> void override
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

        inline auto Has(Entity e) -> bool
        {
            uint32_t idx = GetEntityIndex(e);
            uint32_t p = idx / SPARSE_PAGE_SIZE;

            if (p < m_Sparse.size() && m_Sparse[p])
            {
                uint32_t offset = idx % SPARSE_PAGE_SIZE;
                return (*m_Sparse[p])[offset] != NPOS;
            }

            return false;
        }

        inline auto Get(Entity e) -> T&
        {
            assert(Has(e) && "Failed to retrive component, it doesn't exist on entity");
            uint32_t idx = GetEntityIndex(e);
            uint32_t p = idx / SPARSE_PAGE_SIZE;
            uint32_t offset = idx % SPARSE_PAGE_SIZE;
            uint32_t denseIdx = (*m_Sparse[p])[offset];
            return m_Dense[denseIdx];
        }

        inline auto Get(Entity e) const -> const T&
        {
            assert(Has(e) && "Failed to retrive component, it doesn't exist on entity");
            uint32_t idx = GetEntityIndex(e);
            uint32_t p = idx / SPARSE_PAGE_SIZE;
            uint32_t offset = idx % SPARSE_PAGE_SIZE;
            uint32_t denseIdx = (*m_Sparse[p])[offset];
            return m_Dense[denseIdx];
        }

        inline auto Size() const -> std::size_t
        {
            return m_Dense.size();
        }

        inline auto EntityAt(std::uint32_t i) -> Entity
        {
            return m_DenseEntities[i];
        }

        inline auto Reserve(std::size_t amount) -> void
        {
            std::uint32_t numPages = (amount + SPARSE_PAGE_SIZE - 1) / SPARSE_PAGE_SIZE;

            if (m_Sparse.size() < numPages)
            {
                size_t newSize = numPages;
                size_t oldSize = m_Sparse.size();
                m_Sparse.resize(newSize);

                std::fill(m_Sparse.begin() + oldSize, m_Sparse.end(), nullptr);
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

        inline virtual auto Clear() -> void override
        {
            for (auto page : m_Sparse)
            {
                if (page)
                    page->fill(NPOS);
            }
            m_Dense.clear();
            m_DenseEntities.clear();
        }

        inline auto GetEntities() -> fast_vector<Entity>&
        {
            return m_DenseEntities;
        }

        // Expose raw data for optimization
        inline auto GetDataVector() -> fast_vector<T>&
        {
            return m_Dense;
        }

    private:
        auto SparseSlot(std::uint32_t idx) -> std::uint32_t&
        {
            std::uint32_t p = GetPageIndex(idx);
            if (p >= m_Sparse.size())
            {
                size_t newSize = p + 1;
                size_t oldSize = m_Sparse.size();
                m_Sparse.resize(newSize);

                std::fill(m_Sparse.begin() + oldSize, m_Sparse.end(), nullptr);
            }

            if (!m_Sparse[p])
            {
                m_Sparse[p] = new std::array<std::uint32_t, SPARSE_PAGE_SIZE>();
                m_Sparse[p]->fill(NPOS);
            }

            return (*m_Sparse[p])[GetPageOffset(idx)];
        }

        inline auto GetPageIndex(std::uint32_t idx) const -> const std::uint32_t
        {
            return idx / SPARSE_PAGE_SIZE;
        }

        inline auto GetPageOffset(std::uint32_t idx) const -> const std::uint32_t
        {
            return idx % SPARSE_PAGE_SIZE;
        }

        auto PageFor(std::uint32_t idx) -> std::array<std::uint32_t, SPARSE_PAGE_SIZE>*
        {
            std::uint32_t p = GetPageIndex(idx);
            if (p >= m_Sparse.size())
                return nullptr;
            return m_Sparse[p];
        }

        fast_vector<T> m_Dense;
        fast_vector<Entity> m_DenseEntities;
        fast_vector<std::array<std::uint32_t, SPARSE_PAGE_SIZE>*> m_Sparse;
    };
}
