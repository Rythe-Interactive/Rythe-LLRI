/**
 * @file flags.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
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
        constexpr flags& operator |=(E rhs) noexcept { value = value | rhs; return *this; }

        [[nodiscard]] constexpr flags operator &(E rhs) const noexcept { return value & rhs; }
        constexpr flags& operator &=(E rhs) noexcept { value = value & rhs; return *this; }

        constexpr bool operator ==(flags rhs) const noexcept { return value == rhs.value; }
        constexpr bool operator ==(E rhs) const noexcept { return value == rhs; }

        constexpr bool operator !=(flags rhs) const noexcept { return value != rhs.value; }
        constexpr bool operator !=(E rhs) const noexcept { return value != rhs;}

        constexpr bool operator >(flags rhs) const noexcept { return value > rhs.value; }
        constexpr bool operator >(E rhs) const noexcept { return value > rhs; }

        constexpr bool operator >=(flags rhs) const noexcept { return value >= rhs.value; }
        constexpr bool operator >=(E rhs) const noexcept { return value >= rhs; }

        constexpr bool operator <(flags rhs) const noexcept { return value < rhs.value; }
        constexpr bool operator <(E rhs) const noexcept { return value < rhs; }

        constexpr bool operator <=(flags rhs) const noexcept { return value <= rhs.value; }
        constexpr bool operator <=(E rhs) const noexcept { return value <= rhs; }

        constexpr flags operator ~() const noexcept { return flags{ ~value }; }

        [[nodiscard]] constexpr bool contains(E bit) const noexcept { return (value & bit) == bit; }
        [[nodiscard]] constexpr bool all(flags f) const noexcept { return (value & f.value) == f.value; }
        [[nodiscard]] constexpr bool any(flags f) const noexcept { return (value & f.value) != static_cast<E>(0); }
        [[nodiscard]] constexpr bool none(flags f) const noexcept { return !any(f); }
        constexpr void remove(E bit) noexcept { value = value & ~bit; }
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
    struct hash<llri::flags<E>>
    {
        std::size_t operator()(const llri::flags<E>& f) const
        {
            return hash<typename llri::flags<E>::underlying_t>()(static_cast<typename llri::flags<E>::underlying_t>(f.value));
        }
    };
}
