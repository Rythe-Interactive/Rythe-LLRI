/**
 * @file fence.inl
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> //Recursive include technically not necessary but helps with intellisense

namespace LLRI_NAMESPACE
{
    inline std::string to_string(fence_flag_bits bits)
    {
        switch(bits)
        {
            case fence_flag_bits::None:
                return "None";
            case fence_flag_bits::Signaled:
                return "Signaled";
        }
    }

    inline std::string to_string(fence_flags flags)
    {
        std::string result = "None";

        fence_flags tmp = flags;

        if ((flags & fence_flag_bits::Signaled) == fence_flag_bits::Signaled)
        {
            result += " | Signaled";
            tmp &= ~fence_flag_bits::Signaled;
        }

        if (tmp != fence_flag_bits::None)
            return "Invalid fence_flags value";
        return result;
    }
}
