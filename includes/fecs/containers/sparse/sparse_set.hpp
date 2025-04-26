#pragma once
#include <fecs/core/core.hpp>
#include <vector>

namespace FECS::Container
{

    class ISparseSet
    {
    public:
        /**
 * @brief Virtual destructor for ISparseSet.
 *
 * Ensures proper cleanup of derived sparse set implementations.
 */
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
        /**
 * @brief Destroys the SparseSet and releases its resources.
 */
~SparseSet() = default;

        /**
 * @brief Creates a copy of another SparseSet instance.
 *
 * Performs a shallow copy of the internal storage and mappings from the given SparseSet.
 */
        SparseSet(const SparseSet& other) = default;
        /**
 * @brief Assigns the contents of another SparseSet to this one.
 *
 * Replaces all elements and internal mappings with those from the given SparseSet.
 * Performs a shallow copy of the stored elements and index mappings.
 *
 * @return Reference to this SparseSet.
 */
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
