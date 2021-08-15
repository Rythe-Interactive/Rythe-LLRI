/**
 * @file queue.inl
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> //Recursive include technically not necessary but helps with intellisense

namespace LLRI_NAMESPACE
{
    constexpr const char* to_string(queue_priority priority)
    {
        switch(priority)
        {
            case queue_priority::Normal:
                return "Normal";
            case queue_priority::High:
                return "High";
        }
        return "Invalid queue_priority value";
    }

    constexpr const char* to_string(queue_type type)
    {
        switch(type)
        {
            case queue_type::Graphics:
                return "Graphics";
            case queue_type::Compute:
                return "Compute";
            case queue_type::Transfer:
                return "Transfer";
        }

        return "Invalid queue_type value";
    }
}
