#pragma once
#include <fecs/core/core.h>
#include <vector>

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
    };

    template <typename T>
    class SparseSet : public ISparseSet
    {
    public:
        SparseSet(std::size_t size = 1000);
        ~SparseSet() = default;

        // copy and assignment operators
        SparseSet(const SparseSet& other) = default;
        SparseSet& operator=(const SparseSet& other) = default;

        void Insert(const SparseIndex& index, const T& value);
        virtual void Remove(const SparseIndex& index) override;
        virtual bool Contains(const SparseIndex& index) const override;
        virtual void Resize(std::size_t size) override;
        virtual void Clear() noexcept override;

        T* Get(const SparseIndex& index);
        const std::vector<T>& GetAll() const noexcept;

    private:
        virtual bool IsValidIndex(const SparseIndex& index) const override;

        // members
        std::vector<T> m_Dense;
        std::vector<SparseIndex> m_Sparse;
        std::vector<SparseIndex> m_DenseToSparse;
    };

}

#include "sparse_set.inl"
