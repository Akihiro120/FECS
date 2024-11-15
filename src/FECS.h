#pragma once
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <typeindex>
#include <bitset>
#include <iostream>

#define FECS_MAX_COMPONENTS 32

// an id used to identify an entity
using EntityID = uint32_t;

// base structure for a component
struct Component {

};

// sparse set base
class SparseSetBase {
public:
	 virtual void remove(EntityID id) = 0; 
};

// data structure used to contain components, derived from sparseset
template <typename T>
class SparseSet : public SparseSetBase{
public:
	 void add(EntityID id, T component) {
		  // check if the id is in the registry 
		  if (mSparse.find(id) == mSparse.end()) {
				// add the component to the dense map
				mDense.push_back(component);
				mEntities.push_back(id);

				// add the entity to the sparse map		
				mSparse[id] = mDense.size() - 1;
				return;
		  }
		  std::cout << "Entity ID " << id << " Already Exists in " << typeid(T).name() << std::endl;
	 }

	 void remove(EntityID id) override {
		  if (mSparse.find(id) != mSparse.end()) {
				// get the last and current dense index
				uint32_t dense_index = mSparse[id];
				uint32_t last_index = mDense.size() - 1;

				// swap the last index with the current id
				mDense[dense_index] = mDense[last_index];
				mEntities[dense_index] = mEntities[last_index];

				// replace index
				mSparse[mEntities[last_index]] = dense_index;

				// remove
				mDense.pop_back();
				mEntities.pop_back();
				mSparse.erase(id);
				return;
		  }
		  std::cout << "Entity ID " << id << " doesn't have a " << typeid(T).name() << std::endl;
	 }

	 T* get(EntityID id) {
		  // check if the id is in the registry 
		  if (mSparse.find(id) != mSparse.end()) {
				return &mDense[mSparse[id]];
		  }
		  std::cout << "Component Entry doesnt exist" << std::endl;
		  return nullptr;
	 }

private:
	 std::vector<T> mDense;
	 std::unordered_map<EntityID, uint32_t> mSparse;
	 std::vector<EntityID> mEntities;
};

/*
    FECS is a flexible ECS
*/
class FECS {
public:
    // constructor
    FECS() {
        // debug message initialization
    }

    // add entity
    EntityID add_entity() {
        // create new entity
        EntityID id;
		 
		  if (mFreeIDs.empty()) {
				// there are no free ids
				mEntityIDs.push_back(true);
				id = mEntityIDs.size() - 1;
		  } else {
				// there are free ids
				id = mFreeIDs[mFreeIDs.size() - 1];
				mFreeIDs.pop_back();
				mEntityIDs[id] = true;
		  }
        
        return id;
    }

	 // remove entity
	 void remove_entity(EntityID id) {
		  // remove from all components
		  for (auto& [type_index, set] : mComponents) {
				set->remove(id);
		  }

		  // remove the entity and add it to the free ids
		  mFreeIDs.push_back(id);

		  // remove signature
		  mSignatures.erase(id);

		  // remove
		  mEntityIDs[id] = false;
	 }

	 // add component
	 template <typename T>
	 void add(EntityID id, T component) {
		  // check if the component set exists
		  if (mComponents.find(typeid(T)) == mComponents.end()) {
				// create new sparse set
				mComponents[typeid(T)] = new SparseSet<T>;

				// add component index
				uint32_t index = mComponentIndex.size();
				mComponentIndex[typeid(T)] = index;
		  }

		  // add to sparse set
		  reinterpret_cast<SparseSet<T>*>(mComponents[typeid(T)])->add(id, component);

		  // update signature
		  mSignatures[id] |= 1 << mComponentIndex[typeid(T)];
	 }

	 // remove component
	 template <typename T>
	 void remove(EntityID id) {
		  reinterpret_cast<SparseSet<T>*>(mComponents[typeid(T)])->remove(id);
		  
		  // update signature
		  mSignatures[id] ^= 1 << mComponentIndex[typeid(T)];
	 }

	 // get component
	 template <typename T>
	 T* get(EntityID id) {
		  return reinterpret_cast<SparseSet<T>*>(mComponents[typeid(T)])->get(id);
	 }

	 // query components
	 template <typename... Components, typename Func>
	 void query_system(Func&& fn) {
		  // create a bitset 
		  std::bitset<FECS_MAX_COMPONENTS> required_bit_set;
		  (required_bit_set.set(mComponentIndex[typeid(Components)]), ...);

		  // iterate through each entity and get a list of those who match
		  std::vector<EntityID> entities;
		  for (auto signature : mSignatures) {
				// get matching signatures
				if ((signature.second & required_bit_set) == required_bit_set) {
					 entities.push_back(signature.first);
				}
		  }

		  // update
		  for (int i = 0; i < entities.size(); i++) {
				// apply lambda function
				fn(entities[i], *get<Components>(entities[i])...);
		  }
	 }

	 void terminate() {
		  // clear the sparse sets
		  for (auto& [type, set] : mComponents) {
				delete set;
		  }
	 }

private:
    // ecs related
    std::vector<bool> mEntityIDs;
    std::vector<EntityID> mFreeIDs;
	 
	 // ecs archetypes
	 std::unordered_map<EntityID, std::bitset<FECS_MAX_COMPONENTS>> mSignatures;
	 std::unordered_map<std::type_index, SparseSetBase*> mComponents; 
	 std::unordered_map<std::type_index, uint32_t> mComponentIndex;
};
