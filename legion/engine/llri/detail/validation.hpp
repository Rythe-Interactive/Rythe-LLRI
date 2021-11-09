/**
 * @file validation.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once

#ifdef LLRI_DISABLE_VALIDATION
#define LLRI_DETAIL_VALIDATION_REQUIRE(param, ret)
#define LLRI_DETAIL_VALIDATION_REQUIRE_IT(param, i, ret)
#define LLRI_DETAIL_VALIDATION_REQUIRE_MESSAGE(param, message)
#else
#define LLRI_DETAIL_VALIDATION_REQUIRE(param, ret) { \
        /* make sure the expression is executed only once */ \
        bool requireParamResult = param; \
        if (!requireParamResult) \
        { \
            detail::apiError(__func__, ret, "condition " + std::string(#param) + " was false."); \
            return ret; \
        } \
    }

#define LLRI_DETAIL_VALIDATION_REQUIRE_IT(param, i, ret) { \
        /* make sure the expression is executed only once */ \
        bool requireParamResult = param; \
        if (!requireParamResult) \
        { \
            detail::apiError(__func__, ret, "condition " + std::string(#param) + " (where i == " + std::to_string(i) + ") was false."); \
            return ret; \
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
#else
#define LLRI_DETAIL_CALL_IMPL(func, messenger) \
    const auto r = func; \
    detail::impl_pollAPIMessages(messenger); \
    return r;
#endif
