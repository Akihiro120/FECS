#pragma once
#include "entity.h"
#include <bitset>
#include <iostream>
#include <typeindex>
#include <unordered_map>

// defines
#define FECS_MAX_COMPONENTS 32

// allows for efficient indexing when querying systems using bitsets
class ComponentBitSet {
public:
    // construction
    ComponentBitSet() = default;

    // add component to entity signature
    template <typename T>
    void add(EntityID id) {
        // increment the bit indices if it doesnt exist
        if (m_bit_indexes.find(typeid(T)) == m_bit_indexes.end()) {
            // add
            m_bit_indexes[typeid(T)] = m_bit_indexes.size();
        }
        m_signatures[id] |= 1 << m_bit_indexes[typeid(T)];
    }

    // remove component from entity signature
    template <typename T>
    void remove(EntityID id) {
        // increment the bit indices if it doesnt exist
        if (m_bit_indexes.find(typeid(T)) == m_bit_indexes.end()) {
            // add
            m_bit_indexes[typeid(T)] = m_bit_indexes.size();
        }
        m_signatures[id] ^= 1 << m_bit_indexes[typeid(T)];
    }

    // get the signature of an entity
    auto get(EntityID id) {
        return m_signatures[id];
    }

    // get all signatures
    auto get_signatures() {
        return m_signatures;
    }

    // get all bit indices
    template <typename T>
    auto get_bit_index() {
        // if it doesnt exist
        if (m_bit_indexes.find(typeid(T)) == m_bit_indexes.end()) {
            // add
            m_bit_indexes[typeid(T)] = m_bit_indexes.size();
        }
        return m_bit_indexes[typeid(T)];
    }

private:
    // signature bit sets
    std::unordered_map<EntityID, std::bitset<FECS_MAX_COMPONENTS>> m_signatures;
    std::unordered_map<std::type_index, uint32_t> m_bit_indexes;
};
