#pragma once
#include <vector>
#include <fecs/core/core.hpp>

namespace FECS::Container
{

    template <typename T>
    class SparseSet
    {
    public:
        SparseSet(std::size_t size = 1000);
        ~SparseSet() = default;

        // copy and assignment operators
        SparseSet(const SparseSet& other) = default;
        SparseSet& operator=(const SparseSet& other) = default;

        void Insert(const SparseIndex& index, const T& value);
        void Remove(const SparseIndex& index);
        bool Contains(const SparseIndex& index) const;
        void Resize(std::size_t size);
        void Clear() noexcept;

        T* Get(const SparseIndex& index);
        const std::vector<T>& GetAll() const noexcept;

    private:
        bool IsValidIndex(const SparseIndex& index) const;

        // members
        std::vector<T> m_Dense;
        std::vector<SparseIndex> m_Sparse;
        std::vector<SparseIndex> m_DenseToSparse;
    };

}

#include "sparse_set.inl"
