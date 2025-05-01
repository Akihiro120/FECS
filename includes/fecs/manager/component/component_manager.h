#pragma once
#include <fecs/containers/sparse/sparse_set.h>
#include <fecs/containers/sparse/sparse_set.h>
#include <fecs/core/types.h>
#include <vector>
#include <memory>
#include <unordered_map>

using namespace FECS::Container;
namespace FECS
{
    namespace Manager
    {
        class ComponentManager
        {
        public:
            explicit ComponentManager()
            {
            }
            ~ComponentManager()
            {
            }

            template <typename Component>
            void GetOrRegisterComponent()
            {
            }

            template <typename Component>
            void AttachComponent(Entity entity, Component&& component)
            {
            }

            template <typename Component>
            void DetachComponent(Entity entity)
            {
            }

            template <typename Component>
            Component* Get(Entity entity)
            {
            }

            void ResetSignature(Entity entity)
            {
            }

            // each
            template <typename... Components, typename Function>
            void Each(Function&& queryFunction)
            {
            }

        private:
        };
    }
}
