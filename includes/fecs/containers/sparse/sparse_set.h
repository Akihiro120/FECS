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
        virtual void Remove(const SparseIndex& index) = 0;
        virtual bool Contains(const SparseIndex& index) const = 0;
        virtual void Resize(std::size_t size) = 0;
        virtual void Clear() noexcept = 0;
        virtual bool IsValidIndex(const SparseIndex& index) const = 0;
        virtual std::size_t Size() = 0;
    };

    template <typename T, std::size_t Alignment = 64>
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

        void Insert(const SparseIndex& index, T&& value)
        {
            if (index >= m_Sparse.size())
            {
                m_Sparse.resize(index + 1, INVALID_SPARSE_INDEX);
            }

            // Direct lookup instead of calling contains()
            const std::size_t sparseIndex = m_Sparse[index];

            // Check if item exists
            if (sparseIndex != INVALID_SPARSE_INDEX &&
                sparseIndex < m_DenseToSparse.size() &&
                m_DenseToSparse[sparseIndex] == index)
            {
                // Replace existing item with moved value
                m_Dense[sparseIndex] = std::move(value);
                return;
            }

            // Reserve space if we're approaching capacity
            if (m_Dense.size() == m_Dense.capacity())
            {
                m_Dense.reserve(m_Dense.capacity() * 2);
                m_DenseToSparse.reserve(m_DenseToSparse.capacity() * 2);
            }

            // Insert new item with move
            m_Dense.push_back(std::move(value));
            m_DenseToSparse.emplace_back(index);
            m_Sparse[index] = m_Dense.size() - 1;
        }

        virtual void Remove(const SparseIndex& index) override
        {
            if (!IsValidIndex(index))
            {
                throw std::runtime_error("SparseSet->Remove: Index out of Bounds");
            }

            const std::size_t sparseIndex = m_Sparse[index];

            if (sparseIndex == INVALID_SPARSE_INDEX ||
                sparseIndex >= m_DenseToSparse.size() ||
                m_DenseToSparse[sparseIndex] != index)
            {
                return;
            }

            const std::size_t lastDenseIndex = m_Dense.size() - 1;

            if (sparseIndex == lastDenseIndex)
            {
                m_Sparse[index] = INVALID_SPARSE_INDEX;
                m_Dense.pop_back();
                m_DenseToSparse.pop_back();
                return;
            }

            const std::size_t lastKey = m_DenseToSparse[lastDenseIndex];

            m_Dense[sparseIndex] = std::move(m_Dense[lastDenseIndex]);
            m_DenseToSparse[sparseIndex] = lastKey;
            m_Sparse[lastKey] = sparseIndex;

            m_Sparse[index] = INVALID_SPARSE_INDEX;

            m_Dense.pop_back();
            m_DenseToSparse.pop_back();
        }

        virtual bool Contains(const SparseIndex& index) const override
        {
            if (index >= m_Sparse.size() || index == INVALID_SPARSE_INDEX)
                return false;

            const std::uint32_t sparseIndex = m_Sparse[index];

            if (sparseIndex == INVALID_SPARSE_INDEX)
                return false;

            return sparseIndex < m_DenseToSparse.size() && m_DenseToSparse[sparseIndex] == index;
        }

        virtual void Resize(std::size_t size) override
        {
            // resize
            if (size > m_Sparse.size())
            {
                m_Sparse.resize(size, INVALID_SPARSE_INDEX);
            }
        }

        virtual void Clear() noexcept override
        {
            // clear the sparse ande dense vectors to clear memory
            m_Dense.clear();
            m_DenseToSparse.clear();
            std::fill(m_Sparse.begin(), m_Sparse.end(), INVALID_SPARSE_INDEX);
        }

        virtual std::size_t Size() override
        {
            return m_Dense.size();
        }

        T* Get(const SparseIndex& index)
        {
            if (!IsValidIndex(index))
            {
                throw std::runtime_error("SparseSet->Get: Index out of Bounds");
            }

            // Get the sparse index
            const std::size_t sparseIndex = m_Sparse[index];

            // Check if the element exists
            if (sparseIndex == INVALID_SPARSE_INDEX ||
                sparseIndex >= m_DenseToSparse.size() ||
                m_DenseToSparse[sparseIndex] != index)
            {
                throw std::runtime_error("SparseSet->get: Element not found");
            }

            // Prefetch adjacent elements for locality
            if constexpr (sizeof(T) <= 64)
            {
                // Only prefetch for reasonably sized types
                if (sparseIndex + 4 < m_Dense.size())
                {
                    __builtin_prefetch(&m_Dense[sparseIndex + 1], 0, 3); // Prefetch next element
                    __builtin_prefetch(&m_Dense[sparseIndex + 2], 0, 3);
                    __builtin_prefetch(&m_Dense[sparseIndex + 3], 0, 3);
                }
            }

            return &m_Dense[sparseIndex];
        }

        const T* Get(const SparseIndex& index) const
        {
            if (!IsValidIndex(index))
            {
                throw std::runtime_error("SparseSet->Get: Index out of Bounds");
            }

            // Get the sparse index
            const std::size_t sparseIndex = m_Sparse[index];

            // Check if the element exists
            if (sparseIndex == INVALID_SPARSE_INDEX ||
                sparseIndex >= m_DenseToSparse.size() ||
                m_DenseToSparse[sparseIndex] != index)
            {
                throw std::runtime_error("SparseSet->get: Element not found");
            }

            // Prefetch adjacent elements for locality
            if constexpr (sizeof(T) <= 64)
            { // Only prefetch for reasonably sized types
                if (sparseIndex + 1 < m_Dense.size())
                {
                    __builtin_prefetch(&m_Dense[sparseIndex + 1], 0, 3);
                }
            }

            return &m_Dense[sparseIndex];
        }

        const std::vector<T>& GetAll() const noexcept
        {
            return m_Dense;
        }

    private:
        virtual bool IsValidIndex(const SparseIndex& index) const override
        {
            return index < m_Sparse.size() && index != INVALID_SPARSE_INDEX;
        }

        // members
        std::vector<T> m_Dense;
        std::vector<SparseIndex> m_Sparse;
        std::vector<SparseIndex> m_DenseToSparse;
    };

}
