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
        m_sparse.resize(size, Types::INVALID_SPARSE_INDEX);
    }

    template <typename T>
    void SparseSet<T>::insert(const Types::SparseIndex &index, const T &value)
    {
        assert(is_valid_index(index) && "SparseSet->Insert: Index out of Bounds");

        if (contains(index))
        {
            // replace
            std::size_t sparse_index = m_sparse[index];
            m_dense[sparse_index] = value;
            return;
        }

        m_dense_to_sparse.push_back(index);
        m_dense.push_back(value);
        m_sparse[index] = m_dense.size() - 1;
    }

    template <typename T>
    void SparseSet<T>::remove(const Types::SparseIndex &index)
    {
        assert(is_valid_index(index) && "SparseSet->Remove: Index out of Bounds");

        if (contains(index))
        {
            std::size_t dense_index = m_sparse[index];
            std::size_t last_dense_index = m_dense.size() - 1;
            std::size_t last_key = m_dense_to_sparse[last_dense_index];

            m_dense[dense_index] = m_dense[last_dense_index];
            m_dense_to_sparse[dense_index] = last_key;
            m_sparse[last_key] = dense_index;

            m_dense.pop_back();
            m_dense_to_sparse.pop_back();

            m_sparse[index] = Types::INVALID_SPARSE_INDEX;
        }
    }

    template <typename T>
    bool SparseSet<T>::contains(const Types::SparseIndex &index) const
    {
        return m_sparse[index] != Types::INVALID_SPARSE_INDEX && m_dense_to_sparse[m_sparse[index]] == index;
    }

    template <typename T>
    T *SparseSet<T>::get(const Types::SparseIndex &index)
    {
        assert(is_valid_index(index) && "SparseSet->Get: Index out of Bounds");

        if (contains(index))
        {
            return &m_dense[m_sparse[index]];
        }

        return nullptr;
    }

    template <typename T>
    void SparseSet<T>::resize(std::size_t size)
    {

        // resize
        // TODO: potentially improve the resizing
        m_sparse.resize(size);
    }

    template <typename T>
    void SparseSet<T>::clear() noexcept
    {
        // clear the sparse ande dense vectors to clear memory
        m_dense.clear();
        m_sparse.clear();
        m_dense_to_sparse.clear();
    }

    template <typename T>
    const std::vector<T> &SparseSet<T>::get_all() const noexcept
    {
        return m_dense;
    }

    template <typename T>
    bool SparseSet<T>::is_valid_index(const Types::SparseIndex &index) const
    {
        return index < m_sparse.size();
    }
} // namespace FECS::Container
