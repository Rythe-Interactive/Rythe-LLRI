/**
 * @file validation.hpp
 * Copyright (c) 2021 Leon Brands
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <cstdint>

namespace llri
{
    namespace detail
    {
        /**
         * @brief Returns true if only a single bit is set to 1.
        */
        constexpr bool hasSingleBit(const uint32_t mask) noexcept
        {
            return (mask & (mask - 1)) == 0;
        }

        /**
         * @brief Returns true if the container contains the value
        */
        template <typename C, typename T>
        constexpr bool contains(C container, T value)
        {
            return std::find(container.begin(), container.end(), value) != container.end();
        }
    }
}

#ifdef LLRI_DISABLE_VALIDATION
#define LLRI_DETAIL_VALIDATION_REQUIRE(param, ret)
#define LLRI_DETAIL_VALIDATION_REQUIRE_ITER(param, i, ret)
#define LLRI_DETAIL_VALIDATION_REQUIRE_IF(condition, param, ret)
#define LLRI_DETAIL_VALIDATION_REQUIRE_MESSAGE(param, message)

#else
#define LLRI_DETAIL_ENABLE_VALIDATION

#define LLRI_DETAIL_VALIDATION_REQUIRE(param, ret) { \
        /* make sure the expression is executed only once */ \
        bool requireParamResult = param; \
        if (!requireParamResult) \
        { \
            detail::apiError(__func__, ret, "param " + std::string(#param) + " was false."); \
            return ret; \
        } \
    }

#define LLRI_DETAIL_VALIDATION_REQUIRE_ITER(param, i, ret) { \
        /* make sure the expression is executed only once */ \
        bool requireParamResult = param; \
        if (!requireParamResult) \
        { \
            detail::apiError(__func__, ret, "param " + std::string(#param) + " (where i == " + std::to_string(i) + ") was false."); \
            return ret; \
        } \
    }

#define LLRI_DETAIL_VALIDATION_REQUIRE_IF(condition, param, ret) { \
        if (condition) \
        { \
            /* make sure the expression is executed only once */ \
            bool requireParamResult = param; \
            if (!requireParamResult) \
            { \
                detail::apiError(__func__, ret, std::string(#condition) + " was true and param " + std::string(#param) + " was false."); \
                return ret; \
            } \
        } \
    }

#define LLRI_DETAIL_VALIDATION_REQUIRE_MESSAGE(param, message, ret) { \
        /* make sure the expression is executed only once */ \
        bool requireParamResult = param; \
        if (!requireParamResult) \
        { \
            detail::callUserCallback(message_severity::Error, message_source::API, message); \
            return ret; \
        } \
    }

#endif

#ifdef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
#define LLRI_DETAIL_CALL_IMPL(func, messenger) return func;
#define LLRI_DETAIL_POLL_API_MESSAGES(messenger)
#else
#define LLRI_DETAIL_CALL_IMPL(func, messenger) \
    const auto r = func; \
    detail::impl_pollAPIMessages(messenger); \
    return r;
#define LLRI_DETAIL_POLL_API_MESSAGES(messenger) detail::impl_pollAPIMessages(messenger);
#endif
