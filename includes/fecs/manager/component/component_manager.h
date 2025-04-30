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
        class ComponentTypeIndex
        {
        public:
            template <typename T>
            static ComponentBit GetBitIndex()
            {
                static const ComponentBit id = counter++;
                return id;
            }

        private:
            inline static ComponentBit counter = 0;
        };

        class ComponentManager
        {
        public:
            explicit ComponentManager()
            {
                m_ComponentPool.reserve(COMPONENT_LIMIT);
                m_EntitySignatures.reserve(ENTITY_MINIMUM);
            }
            ~ComponentManager()
            {
            }

            template <typename Component>
            void RegisterComponent()
            {
                const ComponentBit bit = ComponentTypeIndex::GetBitIndex<Component>();

                if (bit >= m_ComponentPool.size())
                {
                    // add new
                    m_ComponentPool.resize(bit + 1);
                }

                if (!m_ComponentPool[bit])
                {
                    auto pool = std::make_unique<SparseSet<Component>>();
                    m_ComponentPool[bit] = std::move(pool);
                }
            }

            template <typename Component>
            void AttachComponent(Entity entity, Component&& component)
            {
                if (entity == INVALID_ENTITY)
                {
                    throw std::runtime_error("FECS Attach: Invalid Entity");
                }

                const ComponentBit bit = ComponentTypeIndex::GetBitIndex<Component>();

                if (entity >= m_EntitySignatures.size())
                {
                    m_EntitySignatures.resize(entity + 1, EMPTY_SIGNATURE);
                }

                if (m_EntitySignatures[entity].test(bit))
                {
                    return;
                }

                GetComponentPool<Component>()->Insert(entity, std::move(component));

                // add the bit
                m_EntitySignatures[entity].set(bit);
            }

            template <typename Component>
            void DetachComponent(Entity entity)
            {
                const ComponentBit bit = ComponentTypeIndex::GetBitIndex<Component>();

                if (bit >= m_ComponentPool.size() || !m_ComponentPool[bit])
                {
                    throw std::runtime_error("FECS Detach: No Such Component Exists");
                }

                // does the bit exist or entity invalid?
                if (!m_EntitySignatures[entity].test(bit) || !IsValidIndex(entity))
                {
                    // doesnt exist return early
                    throw std::runtime_error("FECS Detach: Invalid Entity");
                }

                GetComponentPool<Component>()->Remove(entity);

                // detach signature
                Signature& entitySignature = m_EntitySignatures[entity];
                entitySignature.reset(bit);
            }

            template <typename Component>
            Component* Get(Entity entity)
            {
                const ComponentBit bit = ComponentTypeIndex::GetBitIndex<Component>();

                if (bit >= m_ComponentPool.size() || !m_ComponentPool[bit])
                {
                    throw std::runtime_error("FECS Get: No Such Component Exists");
                }

                return static_cast<SparseSet<Component>*>(m_ComponentPool[bit].get())->Get(entity);
            }

            void ResetSignature(Entity entity)
            {
                if (!IsValidIndex(entity))
                {
                    throw std::runtime_error("FECS Reset: Invalid Entity");
                }

                // no need to remove it, just set it to zero, and reuse
                m_EntitySignatures[entity] = EMPTY_SIGNATURE;
            }

            // each
            template <typename... Components, typename Function>
            void Each(Function&& queryFunction)
            {
                Signature requiredBits;
                (requiredBits.set(ComponentTypeIndex::GetBitIndex<Components>()), ...);

                std::size_t index = 0;
                for (auto& signatures : m_EntitySignatures)
                {
                    if ((signatures & requiredBits) == requiredBits)
                    {
                        queryFunction(index, *GetComponentPool<Components>()->Get(index)...);
                    }

                    index++;
                }
            }

        private:
            bool IsValidIndex(Entity entity) const
            {
                return entity != INVALID_ENTITY && entity < m_EntitySignatures.size();
            }

            template <typename Component>
            inline SparseSet<Component>* GetComponentPool()
            {
                const ComponentBit bit = ComponentTypeIndex::GetBitIndex<Component>();
                return static_cast<SparseSet<Component>*>(m_ComponentPool[bit].get());
            }

            std::vector<std::unique_ptr<ISparseSet>> m_ComponentPool;
            std::vector<Signature> m_EntitySignatures;
        };
    }
}
