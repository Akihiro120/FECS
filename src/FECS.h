#pragma once
#include <bitset>
#include <cstdio>
#include <iostream>
#include <type_traits>
#include <vector>
#include "component.h"
#include "entity.h"
#include "component_pool.h"
#include "component_bitset.h"

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
        EntityID id = m_entity_ids.size();

        // apend to list
        m_entity_ids.push_back(id);
        
        return id;
    }

    // add component
    template <typename T>
    void add(EntityID id, T component) {
        // add the component to the entity
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

        // add to map
        m_component_pool.add<T>(id, component);
        m_component_bitset.add<T>(id);
    }

    // get component
    template <typename T>
    T* get(EntityID id) {
        // static assert
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

        // get the component  
        return m_component_pool.get<T>(id);
    }

    // remove component
    template <typename T>
    void remove(EntityID id) {
        // static assert
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

        // remove the component
        m_component_pool.remove<T>(id);
        m_component_bitset.remove<T>(id);
    }

    // query systems
    template <typename... components, typename func>
    void query_system(func&& fn) {
        // utilise the bitset to get the matching components
        
        // create a bitset
        std::bitset<FECS_MAX_COMPONENTS> required_bit_set;
        (required_bit_set.set(m_component_bitset.get_bit_index<components>()), ...);

        // iterate through each entity and get those who match
        std::vector<EntityID> entities;
        for (auto signature : m_component_bitset.get_signatures()) {
            // get all matching signatures
            if ((signature.second & required_bit_set) == required_bit_set) {
                entities.push_back(signature.first);
            }
        }

        // update each entity and its components
        for (auto entity : entities) {
            // apply the lambda to each component
            fn(*get<components>(entity)...);
        }
    }

    // destructor
    void terminate() {
        // debug message termination
    }

private:

    // ecs related
    std::vector<EntityID> m_entity_ids;
    ComponentPool m_component_pool;
    ComponentBitSet m_component_bitset;
};
