#pragma once
#include <fecs/core/core.hpp>
#include <cstdint>

namespace FECS
{

    // Forward Declaration
    class Registry;
    namespace Manager
    {
        class EntityManager;
    }

    class Entity
    {
    public:
        Entity(Registry& registry, Manager::EntityManager& entityManager);
        ~Entity() = default;

        std::uint32_t GetID() const;
        void SetID(std::uint32_t id);
        bool IsValid() const;

        void Destroy();

        template <typename Component>
        Entity& AttachComponent(const Component& component);
        template <typename Component>
        Entity& DetachComponent();
        template <typename Component>
        Component& Get();

        // Operators
        operator std::uint32_t() const;

    private:
        // References
        Registry& m_RegistryReference;
        Manager::EntityManager& m_EntityManagerReference;

        std::uint32_t m_ID = INVALID_ENTITY_INDEX;
    };

}

#include "entity.inl"
