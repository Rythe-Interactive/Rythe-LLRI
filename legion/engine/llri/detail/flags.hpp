#pragma once
#include <type_traits>

namespace LLRI_NAMESPACE
{
    /**
     * @brief wrapper structure for flags of type E, used to distinguish between x_flag_bits and x_flags.
    */
    template<typename E>
    struct flags
    {
        using U = std::underlying_type_t<E>;
        E value = static_cast<E>(U{});

        constexpr flags() noexcept = default;
        constexpr flags(E val) noexcept : value(val) { }
        [[nodiscard]] constexpr operator E() noexcept { return value; }
        [[nodiscard]] constexpr operator U() noexcept { return static_cast<U>(value); }

        constexpr flags& operator =(E val) noexcept { value = val; return *this; }

        [[nodiscard]] constexpr flags operator |(E rhs) const noexcept { return value | rhs; }
        constexpr flags& operator |=(E rhs) noexcept { value |= rhs; return *this; }

        [[nodiscard]] constexpr flags operator &(E rhs) const noexcept { return value & rhs; }
        constexpr flags& operator &=(E rhs) noexcept { value &= rhs; return *this; }

        constexpr bool operator ==(flags rhs) const noexcept { return value == rhs.value; }
        constexpr bool operator ==(E rhs) const noexcept { return value == rhs; }
    };

    template<typename E>
    [[nodiscard]] constexpr flags<E> operator |(E lhs, flags<E> rhs) noexcept { return lhs | rhs.value; }

    template<typename E>
    [[nodiscard]] constexpr flags<E> operator &(E lhs, flags<E> rhs) noexcept { return lhs & rhs.value; }

    /**
     * @brief Add bitwise operators to flag_bits enum classes.
     */
#define LLRI_DEFINE_FLAG_BIT_OPERATORS(E) \
    constexpr E operator |(E lhs, E rhs) noexcept \
    { \
        using T = std::underlying_type_t<E>; \
        return static_cast<E>(static_cast<T>(lhs) | static_cast<T>(rhs)); \
    }\
    \
    constexpr E operator |=(E lhs, E rhs) noexcept \
    { \
        lhs = lhs | rhs; \
        return lhs; \
    } \
    constexpr E operator &(E lhs, E rhs) noexcept \
    { \
        using T = std::underlying_type_t<E>; \
        return static_cast<E>(static_cast<T>(lhs) & static_cast<T>(rhs)); \
    }\
    \
    constexpr E operator &=(E lhs, E rhs) noexcept \
    { \
        lhs = lhs & rhs; \
        return lhs; \
    } \
    constexpr E operator ~(E lhs) noexcept \
    { \
        using T = std::underlying_type_t<E>; \
        return static_cast<E>(~static_cast<T>(lhs)); \
    }
}

namespace std
{
    template<typename E>
    struct hash<LLRI_NAMESPACE::flags<E>>
    {
        std::size_t operator()(const LLRI_NAMESPACE::flags<E>& f) const
        {
            return hash<typename LLRI_NAMESPACE::flags<E>::U>()(static_cast<typename LLRI_NAMESPACE::flags<E>::U>(f.value));
        }
    };
}
