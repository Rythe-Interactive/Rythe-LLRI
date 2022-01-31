/**
 * @file swapchain_ext.inl
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    inline std::string to_string(present_mode mode)
    {
        switch(mode)
        {
            case present_mode::Immediate:
                return "Immediate";
            case present_mode::Fifo:
                return "Fifo";
            case present_mode::Mailbox:
                return "Mailbox";
        }

        return "Invalid present_mode value";
    }
}
