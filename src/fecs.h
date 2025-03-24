#pragma once
#include "sparse_set.h"
#include <bitset>
#include <typeindex>
#include <unordered_map>

#define MAX_COMPONENTS 100

using Entity = uint32_t;

class FECS {
public:
	FECS() {}
	~FECS() {}

	Entity create_entity() noexcept {
		Entity id;
		if (mFreeEntities.empty()) {
			mNextID++;
			id = mNextID-1;
		} else {
			id = mFreeEntities[mFreeEntities.size() - 1];
			mFreeEntities.pop_back();
		}

		std::bitset<MAX_COMPONENTS> sig;
		mESignatures[id] = sig;
		mSignatures[sig].insert(id, id);
		return id;
	}

	void remove_entity(Entity id) {
		mFreeEntities.push_back(id);

		// remove signatures
		auto it = mESignatures.find(id);
		if (it != mESignatures.end()) {
			auto old = it->second;
			mSignatures[old].remove(id);
			mESignatures.erase(it);
		}
		
		// remove components
		for (auto& storage : mStorage) {
			storage.second->remove(id);
		}
	}

	template <typename T>
	void preregister_component() {
		// register the component if it doesnt yet exist
		if (mCSignature.find(typeid(T)) == mCSignature.end()) {
			
			// create storage 
			mStorage[typeid(T)] = new SparseSet<T>;

			// add signature
			size_t sig = mCSignature.size();
			mCSignature[typeid(T)] = sig;
		}
	}

	template <typename T>
	void attach(Entity id, T component) {
		// register the component if it doesnt yet exist
		if (mCSignature.find(typeid(T)) == mCSignature.end()) {
			
			// create storage 
			mStorage[typeid(T)] = new SparseSet<T>;

			// add signature
			size_t sig = mCSignature.size();
			mCSignature[typeid(T)] = sig;
		}

		// add component to storage
		SparseSet<T>* storage = reinterpret_cast<SparseSet<T>*>(mStorage[typeid(T)]);
		storage->insert(id, component);

		auto e_sig = mESignatures[id];

		mSignatures[e_sig].remove(id);

		auto bit = mCSignature[typeid(T)];
		mESignatures[id].set(bit);
		mSignatures[mESignatures[id]].insert(id, id);
	}

	template <typename T>
	void detach(Entity id) {
		auto it = mCSignature.find(typeid(T));
		if (it != mCSignature.end()) {
			size_t c_index = it->second;
			auto storage = reinterpret_cast<SparseSet<T>*>(mStorage[typeid(T)]);
			storage->remove(id);

			auto& e_sig = mESignatures[id];
			mSignatures[e_sig].remove(id);
			e_sig.reset(c_index);
			mSignatures[e_sig].insert(id, id);
		}
	}

	template <typename T>
	T* get(Entity id) {
		auto it = mStorage.find(typeid(T));
		if (it != mStorage.end()) {
			return reinterpret_cast<SparseSet<T>*>(mStorage[typeid(T)])->get(id);
		}
		return nullptr;
	}

	template <typename... C, typename func>
	void query(func&& fn) {
		std::bitset<MAX_COMPONENTS> required_bits;
		(required_bits.set(mCSignature[typeid(C)]), ...);

		// get the entities
		for (auto& sets : mSignatures) {
			if ((sets.first & required_bits) == required_bits) {
				// get the entities
				for (auto entities : sets.second.get_all()) {
					fn(entities, *get<C>(entities)...);
				}
			}
		}
	}

	void clean() {
		// free memory
		mESignatures.clear();
		mCSignature.clear();
		mStorage.clear();
		mSignatures.clear();
	}

private:
	int mNextID = 0;
	std::vector<Entity> mFreeEntities;
	std::unordered_map<std::type_index, ISparseSet*> mStorage;
	std::unordered_map<std::type_index, size_t> mCSignature;
	std::unordered_map<std::bitset<MAX_COMPONENTS>, SparseSet<Entity>> mSignatures;
	std::unordered_map<Entity, std::bitset<MAX_COMPONENTS>> mESignatures;
};
