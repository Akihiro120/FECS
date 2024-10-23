#pragma once
#include "component.h"
#include "entity.h"
#include <cstdint>
#include <iostream>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

#define MAX_ENTITIES 1000

/**
A sparse set storage system for components
**/
class ComponentPool {
public:
    // construction
    ComponentPool() {
    }

    // insert element
    template <typename T>
    void add(EntityID id, const T& component) {
        // does the component already exist?
        if (m_sparse[id].find(typeid(T)) == m_sparse[id].end()) {
            // add component to dense
            m_components[typeid(T)].push_back(new T(component)); 
            uint32_t component_index = m_components[typeid(T)].size() - 1;

            // sparse will map to the entity ids
            // dense will map to the component ids with m_components
            m_dense.push_back(component_index);
            uint32_t dense_index = m_dense.size() - 1;

            // map to the sparse at entt ID, then store the index to the dense storage
            m_sparse[id][typeid(T)] = dense_index;

            // debug message
            std::cout << "Added Component" << std::endl;
            std::cout << "\tEntity ID: " << id << std::endl;
            std::cout << "\tD-Index: " << dense_index << std::endl;
            std::cout << "\tMap Index: " << typeid(T).name() << std::endl;
            std::cout << "\tC-Map Index: " << component_index << std::endl;
            return;
        }
        std::cout << "Component already Exists?" << std::endl;
    }

    // remove element
    template <typename T>
    void remove(EntityID id) {
        // create iterator
        auto it = m_sparse[id].find(typeid(T));
        if (it != m_sparse[id].end()) {
            // move index and component to the end.
            uint32_t current_sparse_index = m_sparse[id][typeid(T)];
            uint32_t current_dense_index = m_dense[current_sparse_index];
            uint32_t last_index = m_dense.size() - 1;
            uint32_t last_component_index = m_components[typeid(T)].size() - 1;
            Component* component = m_components[typeid(T)][last_component_index];

            // remove that component
            delete m_components[typeid(T)][current_dense_index];
            
            // update the indices of the changed components
            // swap
            m_components[typeid(T)][current_dense_index] = component;

            // update
            m_sparse[id].erase(it); 
            m_dense[current_sparse_index] = current_dense_index;
            m_dense.pop_back();
            m_components[typeid(T)].pop_back();

            return;
        }

        std::cout << "Component doesn't Exist?" << std::endl;
    }

    // get component
    template <typename T>
    T* get(EntityID id) {
        if (m_sparse[id].find(typeid(T)) != m_sparse[id].end()) {
            // get the matching vector for the map
            auto comp_map = m_components[typeid(T)];

            // get the dense mapping
            uint32_t index = m_dense[m_sparse[id][typeid(T)]];

            // return component pointer
            // reinterpret the data
            return reinterpret_cast<T*>(comp_map[index]);
        }
        std::cout << "Component doesn't Exist?" << std::endl;
        return nullptr;
    }

private:
    // storage
    std::unordered_map<EntityID, std::unordered_map<std::type_index, uint32_t>> m_sparse;
    std::vector<uint32_t> m_dense;
    std::unordered_map<std::type_index, std::vector<Component*>> m_components;
};
