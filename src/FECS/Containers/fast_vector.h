#pragma once

#include <cassert>
#include <cstdlib>
#include <cstring> // std::memcpy()
#include <new>
#include <type_traits>

// Helper functions

template <class T>
inline auto construct_range(T* begin, T* end) -> void
{
    while (begin != end)
    {
        new (begin) T;
        begin++;
    }
}

template <class T>
inline auto copy_range(T* begin, T* end, T* dest) -> void
{
    while (begin != end)
    {
        new (dest) T(*begin);
        begin++;
        dest++;
    }
}

template <class T>
inline auto destruct_range(T* begin, T* end) -> void
{
    while (begin != end)
    {
        begin->~T();
        begin++;
    }
}

template <class T>
class fast_vector
{
public:
    using size_type = std::size_t;

    fast_vector() = default;
    fast_vector(const fast_vector& other);
    fast_vector(fast_vector&& other) noexcept;
    fast_vector& operator=(const fast_vector& other);
    fast_vector& operator=(fast_vector&& other) noexcept;
    ~fast_vector();

    // Element access

    auto operator[](size_type pos) -> T&;
    auto operator[](size_type pos) const -> const T&;

    auto front() -> T&;
    auto front() const -> const T&;

    auto back() -> T&;
    auto back() const -> const T&;

    auto data() noexcept -> T*;
    auto data() const noexcept -> const T*;

    // Iterators

    auto begin() noexcept -> T*;
    auto begin() const noexcept -> const T*;

    auto end() noexcept -> T*;
    auto end() const noexcept -> const T*;

    // Capacity

    auto empty() const noexcept -> bool;
    auto size() const noexcept -> size_type;
    auto reserve(size_type new_cap) -> void;
    auto capacity() const noexcept -> size_type;
    auto shrink_to_fit() -> void;

    // Modifiers

    auto clear() noexcept -> void;

    auto push_back(const T& value) -> void;
    auto push_back(T&& value) -> void;

    template <class... Args>
    auto emplace_back(Args&&... args) -> void;

    auto pop_back() -> void;
    auto resize(size_type count) -> void;

    static constexpr size_type grow_factor = 2;

private:
    T* m_data = nullptr;
    size_type m_size = 0;
    size_type m_capacity = 0;
};

template <class T>
fast_vector<T>::fast_vector(const fast_vector& other)
    : m_size(other.m_size), m_capacity(other.m_capacity)
{
    m_data = reinterpret_cast<T*>(std::malloc(sizeof(T) * other.m_capacity));

    if (std::is_trivial_v<T>)
    {
        std::memcpy(m_data, other.m_data, other.m_size);
    }
    else
    {
        copy_range(other.begin(), other.end(), m_data);
    }
}

template <class T>
fast_vector<T>::fast_vector(fast_vector&& other) noexcept
    : m_data(other.m_data), m_size(other.m_size), m_capacity(other.m_capacity)
{
    other.m_data = nullptr;
}

template <class T>
fast_vector<T>& fast_vector<T>::operator=(const fast_vector& other)
{
    m_size = other.m_size;
    m_capacity = other.m_capacity;

    m_data = reinterpret_cast<T*>(std::malloc(sizeof(T) * other.m_capacity));

    if (std::is_trivial_v<T>)
    {
        std::memcpy(m_data, other.m_data, other.m_size);
    }
    else
    {
        copy_range(other.begin(), other.end(), m_data);
    }

    return *this;
}

template <class T>
fast_vector<T>& fast_vector<T>::operator=(fast_vector&& other) noexcept
{
    m_data = other.m_data;
    m_size = other.m_size;
    m_capacity = other.m_capacity;

    other.m_data = nullptr;

    return *this;
}

template <class T>
fast_vector<T>::~fast_vector()
{
    if (!std::is_trivial_v<T>)
    {
        destruct_range(begin(), end());
    }

    std::free(m_data);
}

// Element access

template <class T>
auto fast_vector<T>::operator[](size_type pos) -> T&
{
    assert(pos < m_size && "Position is out of range");
    return m_data[pos];
}

template <class T>
auto fast_vector<T>::operator[](size_type pos) const -> const T&
{
    assert(pos < m_size && "Position is out of range");
    return m_data[pos];
}

template <class T>
auto fast_vector<T>::front() -> T&
{
    assert(m_size > 0 && "Container is empty");
    return m_data[0];
}

template <class T>
auto fast_vector<T>::front() const -> const T&
{
    assert(m_size > 0 && "Container is empty");
    return m_data[0];
}

template <class T>
auto fast_vector<T>::back() -> T&
{
    assert(m_size > 0 && "Container is empty");
    return m_data[m_size - 1];
}

template <class T>
auto fast_vector<T>::back() const -> const T&
{
    assert(m_size > 0 && "Container is empty");
    return m_data[m_size - 1];
}

template <class T>
auto fast_vector<T>::data() noexcept -> T*
{
    return m_data;
}

template <class T>
auto fast_vector<T>::data() const noexcept -> const T*
{
    return m_data;
}

// Iterators

template <class T>
auto fast_vector<T>::begin() noexcept -> T*
{
    return m_data;
}

template <class T>
auto fast_vector<T>::begin() const noexcept -> const T*
{
    return m_data;
}

template <class T>
auto fast_vector<T>::end() noexcept -> T*
{
    return m_data + m_size;
}

template <class T>
auto fast_vector<T>::end() const noexcept -> const T*
{
    return m_data + m_size;
}

// Capacity

template <class T>
auto fast_vector<T>::empty() const noexcept -> bool
{
    return m_size == 0;
}

template <class T>
auto fast_vector<T>::size() const noexcept -> typename fast_vector<T>::size_type
{
    return m_size;
}

template <class T>
auto fast_vector<T>::reserve(size_type new_cap) -> void
{
    assert(new_cap > m_capacity && "Capacity is already equal to or greater than the passed value");

    if constexpr (std::is_trivial_v<T>)
    {
        m_data = reinterpret_cast<T*>(std::realloc(m_data, sizeof(T) * new_cap));
        assert(m_data != nullptr && "Reallocation failed");
    }
    else
    {
        T* new_data_location = reinterpret_cast<T*>(std::malloc(sizeof(T) * new_cap));
        assert(new_data_location != nullptr && "Allocation failed");

        copy_range(begin(), end(), new_data_location);
        destruct_range(begin(), end());

        std::free(m_data);

        m_data = new_data_location;
    }

    m_capacity = new_cap;
}

template <class T>
auto fast_vector<T>::capacity() const noexcept -> typename fast_vector<T>::size_type
{
    return m_capacity;
}

template <class T>
auto fast_vector<T>::shrink_to_fit() -> void
{
    if (m_size < m_capacity)
    {
        if constexpr (std::is_trivial_v<T>)
        {
            m_data = reinterpret_cast<T*>(std::realloc(m_data, sizeof(T) * m_size));
            assert(m_data != nullptr && "Reallocation failed");
        }
        else
        {
            T* new_data_location = reinterpret_cast<T*>(std::malloc(sizeof(T) * m_size));
            assert(new_data_location != nullptr && "Allocation failed");

            copy_range(begin(), end(), new_data_location);
            destruct_range(begin(), end());

            std::free(m_data);

            m_data = new_data_location;
        }
    }
}

// Modifiers

template <class T>
auto fast_vector<T>::clear() noexcept -> void
{
    if constexpr (!std::is_trivial_v<T>)
    {
        destruct_range(begin(), end());
    }

    m_size = 0;
}

template <class T>
auto fast_vector<T>::push_back(const T& value) -> void
{
    if (m_size == m_capacity)
    {
        reserve(m_capacity * fast_vector::grow_factor + 1);
    }

    if constexpr (std::is_trivial_v<T>)
    {
        m_data[m_size] = value;
    }
    else
    {
        new (m_data + m_size) T(value);
    }

    m_size++;
}

template <class T>
auto fast_vector<T>::push_back(T&& value) -> void
{
    if (m_size == m_capacity)
    {
        reserve(m_capacity * fast_vector::grow_factor + 1);
    }

    if constexpr (std::is_trivial_v<T>)
    {
        m_data[m_size] = value;
    }
    else
    {
        new (m_data + m_size) T(std::move(value));
    }

    m_size++;
}

template <class T>
template <class... Args>
auto fast_vector<T>::emplace_back(Args&&... args) -> void
{
    static_assert(!std::is_trivial_v<T>, "Use push_back() instead of emplace_back() with trivial types");

    if (m_size == m_capacity)
    {
        reserve(m_capacity * fast_vector::grow_factor + 1);
    }

    new (m_data + m_size) T(std::forward<Args>(args)...);

    m_size++;
}

template <class T>
auto fast_vector<T>::pop_back() -> void
{
    assert(m_size > 0 && "Container is empty");

    if constexpr (!std::is_trivial_v<T>)
    {
        m_data[m_size - 1].~T();
    }

    m_size--;
}

template <class T>
auto fast_vector<T>::resize(size_type count) -> void
{
    assert(count != m_size && "Size is already equal to the passed value");

    if (count > m_capacity)
    {
        reserve(count);
    }

    if constexpr (!std::is_trivial_v<T>)
    {
        if (count > m_size)
        {
            construct_range(m_data + m_size, m_data + count);
        }
        else if (count < m_size)
        {
            destruct_range(m_data + count, m_data + m_size);
        }
    }

    m_size = count;
}
