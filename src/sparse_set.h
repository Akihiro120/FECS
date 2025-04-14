#pragma once
#include <cassert>
#include <cstddef>
#include <limits>
#include <vector>

#define MAX_SPARSE_SIZE 2000

class ISparseSet {
public:
	ISparseSet() = default;

	virtual void remove(size_t index) = 0;
};

template <typename T>
class SparseSet : public ISparseSet {
public:
	// constructor, deconstructor
	SparseSet() {
		mSparse.resize(MAX_SPARSE_SIZE, INVALID_INDEX);
	}

	void insert(size_t index, T entry) {
		assert(index < MAX_SPARSE_SIZE);

		if (contains(index)) {
			// replace
			size_t sparse_index = mSparse[index];
			mDense[sparse_index] = entry;
			return;
		}


		mDenseIndices.push_back(index);
		mDense.push_back(entry);
		mSparse[index] = mDense.size() - 1;
	}

	virtual void remove(size_t sparse_index) override {
		assert(sparse_index < MAX_SPARSE_SIZE);

		if (!contains(sparse_index)) {
			return;
		}

		size_t dense_index = mSparse[sparse_index];	
		size_t last_dense_index = mDense.size() - 1;
		size_t last_key = mDenseIndices[last_dense_index];

		mDense[dense_index] = mDense[last_dense_index];
		mDenseIndices[dense_index] = last_key; 
		mSparse[last_key] = dense_index;

		mDense.pop_back();
		mDenseIndices.pop_back();
		mSparse[sparse_index] = INVALID_INDEX;
	}

	T* get(size_t index) {
		assert(index < MAX_SPARSE_SIZE);

		// get the entry at index
		if (!contains(index)) {
			return nullptr;
		}
			
		int sparse_index = mSparse[index];
		return &mDense[sparse_index];
	}

	// READ ONLY
	const std::vector<T>& get_all() const noexcept {
		return mDense;
	}

private:
	// containers, dense, and sparse
	std::vector<T> mDense;
	std::vector<size_t> mSparse;
	std::vector<size_t> mDenseIndices;
	size_t INVALID_INDEX = std::numeric_limits<size_t>::max();

	bool contains(size_t index) {
		// assert(mSparse.size() > index);

		return mSparse[index] != INVALID_INDEX && mDenseIndices[mSparse[index]] == index;
	}
};
