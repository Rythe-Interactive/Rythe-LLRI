/**
 * @file swapchain_ext.inl
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    inline std::string to_string(present_mode_ext mode)
    {
        switch(mode)
        {
            case present_mode_ext::Immediate:
                return "Immediate";
            case present_mode_ext::Fifo:
                return "Fifo";
            case present_mode_ext::Mailbox:
                return "Mailbox";
        }

        return "Invalid present_mode_ext value";
    }

    inline swapchain_desc_ext SwapchainEXT::getDesc() const
    {
        return m_desc;
    }

    inline SwapchainEXT::native_swapchain_ext* SwapchainEXT::getNative() const
    {
        return m_ptr;
    }
}
