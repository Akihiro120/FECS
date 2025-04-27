#pragma once
#include <bitset>
#include <unordered_map>
#include <typeindex>
#include <fecs/containers/sparse/sparse_set.h>

namespace FECS
{

    namespace Manager
    {
        class ComponentManager;
    }

    namespace Manager
    {

        using namespace FECS::Container;
        using Signature = std::bitset<MAX_COMPONENTS>;

        class QueryManager
        {
        public:
            QueryManager(ComponentManager& componentManager);
            ~QueryManager() = default;

            void CreateEntitySignature(std::uint32_t id);
            void RemoveEntitySignature(std::uint32_t id);
            void AttachEntitySignature(std::uint32_t id, const std::type_index& typeID);
            void DetachEntitySignature(std::uint32_t id, const std::type_index& typeID);
            void CreateComponentSignature(const std::type_index& typeID);

            // querying
            template <typename... Components, typename Function>
            void Each(Function&& queryFunction);

        private:
            void VerifyID(std::uint32_t id, const std::string& msg = "");

            std::unordered_map<Signature, SparseSet<std::uint32_t>> m_Queries;
            std::unordered_map<std::type_index, size_t> m_ComponentBit;
            std::unordered_map<std::uint32_t, Signature> m_EntitySignatures;

            ComponentManager& m_ComponentManagerReference;
        };
    }
}

#include "query_manager.inl"
