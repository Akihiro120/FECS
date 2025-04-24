#pragma once
#include <vector>
#include "../utils/types.hpp"

namespace FECS::Container
{
    template <typename T>
    class SparseSet
    {
    public:
        SparseSet(std::size_t size = 1000);
        ~SparseSet() = default;

        // copy and assignment operators
        SparseSet(const SparseSet &other) = default;
        SparseSet &operator=(const SparseSet &other) = default;

        void insert(const Types::SparseIndex &index, const T &value);
        void remove(const Types::SparseIndex &index);
        bool contains(const Types::SparseIndex &index) const;
        void resize(std::size_t size);
        void clear() noexcept;

        T *get(const Types::SparseIndex &index);
        const std::vector<T> &get_all() const noexcept;

    private:
        bool is_valid_index(const Types::SparseIndex &index) const;

        // members
        std::vector<T> m_dense;
        std::vector<Types::SparseIndex> m_sparse;
        std::vector<Types::SparseIndex> m_dense_to_sparse;
    };
} // namespace FECS::Container

#include "sparse_set.inl"
