#pragma once

#include <tuple>
#include <vector>
#include <cstddef>
#include <utility>
#include <fecs/containers/sparse_set.h>
#include <fecs/manager/entity_manager.h>
#include <fecs/manager/component_manager.h>

namespace FECS::Manager
{

    template <typename... Cs>
    class Group
    {

    public:
        // Constructor: take your EntityManager* and fetch each pool pointer
        explicit Group(EntityManager* mgr)
        {
        }

        // Rebuild the entity list from scratch, driven by the *first* pool
        void Rebuild()
        {
        }

        template <typename F>
        void Each(F&& func)
        {
        }
    };

} // namespace FECS::Manager
