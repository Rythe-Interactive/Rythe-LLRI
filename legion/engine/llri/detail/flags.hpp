/**
 * @file flags.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

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
        using underlying_t = std::underlying_type_t<E>;
        E value = static_cast<E>(underlying_t{});

        constexpr flags() noexcept = default;
        constexpr flags(E val) noexcept : value(val) { }
        constexpr flags(underlying_t val) noexcept : value(static_cast<E>(val)) { }
        [[nodiscard]] constexpr operator E() noexcept { return value; }
        [[nodiscard]] constexpr operator underlying_t() noexcept { return static_cast<underlying_t>(value); }

        constexpr flags& operator =(E val) noexcept { value = val; return *this; }

        [[nodiscard]] constexpr flags operator |(E rhs) const noexcept { return value | rhs; }
        constexpr flags& operator |=(E rhs) noexcept { value |= rhs; return *this; }

        [[nodiscard]] constexpr flags operator &(E rhs) const noexcept { return value & rhs; }
        constexpr flags& operator &=(E rhs) noexcept { value = value & rhs; return *this; }

        constexpr bool operator ==(flags rhs) const noexcept { return value == rhs.value; }
        constexpr bool operator ==(E rhs) const noexcept { return value == rhs; }

        [[nodiscard]] constexpr bool contains(E flag) const noexcept { return (value & flag) == flag; }
        constexpr void remove(E flag) noexcept { value = value & ~flag; }
    };

    template<typename E>
    [[nodiscard]] constexpr flags<E> operator |(E lhs, flags<E> rhs) noexcept { return lhs | rhs.value; }

    template<typename E>
    [[nodiscard]] constexpr flags<E> operator &(E lhs, flags<E> rhs) noexcept { return lhs & rhs.value; }

    /**
     * @brief Add bitwise operators to flag_bits enum classes.
     */
#define LLRI_DEFINE_FLAG_BIT_OPERATORS(E) \
    constexpr flags<E> operator |(E lhs, E rhs) noexcept \
    { \
        using T = std::underlying_type_t<E>; \
        return static_cast<flags<E>>(static_cast<T>(lhs) | static_cast<T>(rhs)); \
    }\
    constexpr flags<E> operator &(E lhs, E rhs) noexcept \
    { \
        using T = std::underlying_type_t<E>; \
        return static_cast<flags<E>>(static_cast<T>(lhs) & static_cast<T>(rhs)); \
    }\
    constexpr flags<E> operator ~(E lhs) noexcept \
    { \
        using T = std::underlying_type_t<E>; \
        return static_cast<flags<E>>(~static_cast<T>(lhs)); \
    }
}

namespace std
{
    template<typename E>
    struct hash<LLRI_NAMESPACE::flags<E>>
    {
        std::size_t operator()(const LLRI_NAMESPACE::flags<E>& f) const
        {
            return hash<typename LLRI_NAMESPACE::flags<E>::underlying_t>()(static_cast<typename LLRI_NAMESPACE::flags<E>::underlying_t>(f.value));
        }
    };
}
