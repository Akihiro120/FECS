#pragma once
#include <fecs/core/types.h>
#include <vector>
#include <cassert>
#include <stdexcept>

namespace FECS::Container
{

    class ISparseSet
    {
    public:
        virtual ~ISparseSet() = default;
        virtual void Remove(SparseIndex index) = 0;
    };

    template <typename T>
    class SparseSet : public ISparseSet
    {
    public:
        explicit SparseSet()
        {
            // size
            std::size_t size = SPARSE_MINIMUM_SIZE;

            // initialize the sparse and dense vectors
            m_Sparse.resize(size, INVALID_SPARSE_INDEX);
            m_Dense.reserve(size);
            m_DenseToSparse.reserve(size);
        }
        ~SparseSet() = default;

        // copy and assignment operators
        SparseSet(const SparseSet& other) = default;
        SparseSet& operator=(const SparseSet& other) = default;

        void Insert(SparseIndex index, T&& value)
        {
            if (index >= m_Sparse.size())
            {
                m_Sparse.resize(index + 1, INVALID_SPARSE_INDEX);
            }

            const SparseIndex sparseIndex = m_Sparse[index];
            if (sparseIndex != INVALID_SPARSE_INDEX &&
                m_DenseToSparse[sparseIndex] == index)
            {
                return;
            }

            // Insert new item with move
            m_Dense.emplace_back(std::move(value));
            m_DenseToSparse.emplace_back(index);
            m_Sparse[index] = m_Dense.size() - 1;
        }

        template <typename Container>
        void BatchInsert(const Container& items)
        {
            if (items.empty())
                return;

            SparseIndex maxIndex = 0;
            for (const auto& [index, _] : items)
            {
                maxIndex = std::max(maxIndex, index);
            }

            // Resize once
            EnsureCapacity(maxIndex);

            const std::size_t newSize = m_Dense.size() + items.size();
            m_Dense.reserve(newSize);
            m_DenseToSparse.reserve(newSize);

            for (const auto& [index, value] : items)
            {
                const SparseIndex sparseIndex = m_Sparse[index];
                if (sparseIndex != INVALID_SPARSE_INDEX &&
                    m_DenseToSparse[sparseIndex] == index)
                    continue;

                m_Dense.emplace_back(std::move(value));
                m_DenseToSparse.emplace_back(index);
                m_Sparse[index] = static_cast<SparseIndex>(m_Dense.size() - 1);
            }
        }

        virtual void Remove(SparseIndex index) override
        {
            if (!IsValidIndex(index))
            {
                throw std::runtime_error("SparseSet->Remove: Invalid Index");
            }

            const SparseIndex sparseIndex = m_Sparse[index];
            const SparseIndex lastDenseIndex = m_Dense.size() - 1;

            if (sparseIndex != lastDenseIndex)
            {
                const SparseIndex lastKey = m_DenseToSparse[lastDenseIndex];
                if constexpr (std::is_trivially_copyable_v<T>)
                    m_Dense[sparseIndex] = m_Dense[lastDenseIndex];
                else
                    m_Dense[sparseIndex] = std::move(m_Dense[lastDenseIndex]);

                m_DenseToSparse[sparseIndex] = lastKey;
                m_Sparse[lastKey] = sparseIndex;
            }

            m_Sparse[index] = INVALID_SPARSE_INDEX;
            m_Dense.pop_back();
            m_DenseToSparse.pop_back();
        }

        inline bool Has(SparseIndex index) const
        {
            if (!IsValidIndex(index))
            {
                throw std::runtime_error("SparseSet->Has: Invalid Index");
            }

            const SparseIndex sparseIndex = m_Sparse[index];
            return sparseIndex < m_DenseToSparse.size() && m_DenseToSparse[sparseIndex] == index;
        }

        void Resize(std::size_t size)
        {
            // resize
            if (size > m_Sparse.size())
            {
                m_Sparse.resize(size, INVALID_SPARSE_INDEX);
            }
        }

        void Clear() noexcept
        {
            // clear the sparse ande dense vectors to clear memory
            m_Dense.clear();
            m_DenseToSparse.clear();
            std::fill(m_Sparse.begin(), m_Sparse.end(), INVALID_SPARSE_INDEX);
        }

        const std::size_t Size() const
        {
            return m_Dense.size();
        }

        T* Get(SparseIndex index)
        {
            // Get the sparse index
            return &m_Dense[m_Sparse[index]];
        }

        const T* Get(SparseIndex index) const
        {
            // Get the sparse index
            return &m_Dense[m_Sparse[index]];
        }

        const std::vector<T>& GetAll() const
        {
            return m_Dense;
        }

        // iteration
        auto begin() const
        {
            return m_Dense.begin();
        }
        auto end() const
        {
            return m_Dense.end();
        }

    private:
        inline void EnsureCapacity(SparseIndex index)
        {
            if (index >= m_Sparse.size())
                m_Sparse.resize(index + 1, INVALID_SPARSE_INDEX);
        }

        inline bool IsValidIndex(SparseIndex index) const
        {
            return index < m_Sparse.size() && index != INVALID_SPARSE_INDEX;
        }

        // members
        std::pmr::vector<T> m_Dense;
        std::pmr::vector<SparseIndex> m_Sparse;
        std::pmr::vector<SparseIndex> m_DenseToSparse;
    };

}
