#include <fecs/manager/query/query_manager.h>
#include <fecs/core/core.h>
#include <stdexcept>

namespace FECS
{

    namespace Manager
    {

        QueryManager::QueryManager(ComponentManager& componentManager)
            : m_ComponentManagerReference(componentManager)
        {
        }

        void QueryManager::CreateEntitySignature(std::uint32_t id)
        {
            VerifyID(id, "CreateEntitySignature");

            Signature emptySignature;
            m_EntitySignatures[id] = emptySignature;
            m_Queries[emptySignature].Insert(id, id);
        }

        void QueryManager::RemoveEntitySignature(std::uint32_t id)
        {
            VerifyID(id, "RemoveEntitySignature");

            auto it = m_EntitySignatures.find(id);
            if (it != m_EntitySignatures.end())
            {
                auto old = it->second;
                m_Queries[old].Remove(id);
                m_EntitySignatures.erase(it);
            }
        }

        void QueryManager::AttachEntitySignature(std::uint32_t id, const std::type_index& typeID)
        {
            VerifyID(id, "AttachEntitySignature");

            auto it = m_EntitySignatures.find(id);
            if (it != m_EntitySignatures.end())
            {
                // remove the old signature
                Signature oldEntitySignature = m_EntitySignatures[id];
                m_Queries[oldEntitySignature].Remove(id);

                // get the component bit
                CreateComponentSignature(typeID);
                auto bit = m_ComponentBit[typeID];

                // add the new Signature
                Signature& newEntitySignature = m_EntitySignatures[id];
                newEntitySignature.set(bit);
                m_Queries[newEntitySignature].Insert(id, id);
            }
        }

        void QueryManager::DetachEntitySignature(std::uint32_t id, const std::type_index& typeID)
        {
            VerifyID(id, "DetachEntitySignature");

            auto it = m_EntitySignatures.find(id);
            if (it != m_EntitySignatures.end())
            {
                // remove the old signature
                Signature oldEntitySignature = m_EntitySignatures[id];
                m_Queries[oldEntitySignature].Remove(id);

                // get the component bit
                CreateComponentSignature(typeID);
                auto bit = m_ComponentBit[typeID];

                // add the new Signature
                Signature& newEntitySignature = m_EntitySignatures[id];
                newEntitySignature.reset(bit);
                m_Queries[newEntitySignature].Insert(id, id);
            }
        }

        void QueryManager::VerifyID(std::uint32_t id, const std::string& msg)
        {
            if (id == INVALID_ENTITY_INDEX)
            {
                throw std::runtime_error("QueryManager->" + msg + ": Invaid Entity ID");
            }
        }

        void QueryManager::CreateComponentSignature(const std::type_index& typeID)
        {
            auto it = m_ComponentBit.find(typeID);
            if (it == m_ComponentBit.end())
            {
                std::uint32_t bit = static_cast<std::uint32_t>(m_ComponentBit.size());
                m_ComponentBit[typeID] = bit;
            }
        }
    }
}
