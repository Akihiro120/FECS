#pragma once
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <type_traits>

class ResourceIndex
{
public:
    template <typename T>
    static auto GetIndex() -> std::uint32_t
    {
        static const std::uint32_t idx = m_Index++;
        return idx;
    }

private:
    static inline std::uint32_t m_Index = 0;
};

namespace FECS::Manager
{
    class ResourceManager
    {
    public:
        ResourceManager() = default;
        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;

        template <typename T, typename... Args>
        auto Emplace(Args&&... args) -> T&
        {
            const auto typeId = ResourceIndex::GetIndex<T>();

            std::shared_ptr<T> resource = std::make_shared<T>(std::forward<Args>(args)...);

            m_Resources[typeId] = resource;

            return *resource;
        }

        template <typename T>
        auto Add(T&& resource) -> void
        {
            Emplace<std::remove_reference_t<T>>(std::forward<T>(resource));
        }

        template <typename T>
        auto Has() const -> bool
        {
            const auto typeId = ResourceIndex::GetIndex<T>();
            return m_Resources.find(typeId) != m_Resources.end();
        }

        template <typename T>
        auto Get() -> T&
        {
            const auto typeId = ResourceIndex::GetIndex<T>();
            auto it = m_Resources.find(typeId);

            if (it == m_Resources.end())
            {
                throw std::runtime_error("ResourceManager: Attempted to access missing resource.");
            }

            return *std::static_pointer_cast<T>(it->second);
        }

    private:
        std::unordered_map<std::uint32_t, std::shared_ptr<void>> m_Resources;
    };
}
