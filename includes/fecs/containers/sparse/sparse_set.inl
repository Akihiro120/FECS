#pragma once
#include "sparse_set.hpp"
#include <vector>
#include <cassert>

namespace FECS::Container
{

    template <typename T>
    SparseSet<T>::SparseSet(std::size_t size)
    {
        // initialize the sparse and dense vectors
        m_Sparse.resize(size, INVALID_SPARSE_INDEX);
    }

    template <typename T>
    void SparseSet<T>::Insert(const SparseIndex &index, const T &value)
    {
        assert(IsValidIndex(index) && "SparseSet->Insert: Index out of Bounds");

        if (Contains(index))
        {
            // replace
            std::size_t sparseIndex = m_Sparse[index];
            m_Dense[sparseIndex] = value;
            return;
        }

        m_DenseToSparse.push_back(index);
        m_Dense.push_back(value);
        m_Sparse[index] = m_Dense.size() - 1;
    }

    template <typename T>
    void SparseSet<T>::Remove(const SparseIndex &index)
    {
        assert(IsValidIndex(index) && "SparseSet->Remove: Index out of Bounds");

        if (Contains(index))
        {
            std::size_t denseIndex = m_Sparse[index];
            std::size_t lastDenseIndex = m_Dense.size() - 1;
            std::size_t lastKey = m_DenseToSparse[lastDenseIndex];

            m_Dense[denseIndex] = m_Dense[lastDenseIndex];
            m_DenseToSparse[denseIndex] = lastKey;
            m_Sparse[lastKey] = denseIndex;

            m_Dense.pop_back();
            m_DenseToSparse.pop_back();

            m_Sparse[index] = INVALID_SPARSE_INDEX;
        }
    }

    template <typename T>
    bool SparseSet<T>::Contains(const SparseIndex &index) const
    {
        return m_Sparse[index] != INVALID_SPARSE_INDEX &&
               m_Sparse[index] < m_DenseToSparse.size() &&
               m_DenseToSparse[m_Sparse[index]] == index;
    }

    template <typename T>
    T *SparseSet<T>::Get(const SparseIndex &index)
    {
        assert(IsValidIndex(index) && "SparseSet->Get: Index out of Bounds");

        if (Contains(index))
        {
            return &m_Dense[m_Sparse[index]];
        }

        return nullptr;
    }

    template <typename T>
    void SparseSet<T>::Resize(std::size_t size)
    {

        // resize
        if (size > m_Sparse.size())
        {
            m_Sparse.resize(size, INVALID_SPARSE_INDEX);
        }
    }

    template <typename T>
    void SparseSet<T>::Clear() noexcept
    {
        // clear the sparse ande dense vectors to clear memory
        m_Dense.clear();
        m_Sparse.clear();
        m_DenseToSparse.clear();
    }

    template <typename T>
    const std::vector<T> &SparseSet<T>::GetAll() const noexcept
    {
        return m_Dense;
    }

    template <typename T>
    bool SparseSet<T>::IsValidIndex(const SparseIndex &index) const
    {
        return index < m_Sparse.size();
    }
} // namespace FECS::Container
