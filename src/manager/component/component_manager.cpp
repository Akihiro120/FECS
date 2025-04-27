#include <fecs/manager/component/component_manager.h>

namespace FECS
{

    namespace Manager
    {

        ComponentManager::ComponentManager(QueryManager& queryManager)
            : m_QueryManagerReference(queryManager)
        {
        }

        void ComponentManager::RemoveEntity(std::uint32_t id)
        {
            for (auto& [key, storage] : m_ComponentPool)
            {
                storage->Remove(id);
            }
        }
    }
}
