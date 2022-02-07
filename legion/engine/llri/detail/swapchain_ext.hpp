/**
 * @file swapchain_ext.hpp
 * Copyright (c) 2021 Leon Brands
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    /**
     * @brief Describes how the presentation engine interacts with a swapchain's textures and the surface.
    */
    enum struct present_mode_ext : uint8_t
    {
        /**
         * @brief The presentation engine doesn't wait for vertical sync, which may result in visible tearing.
        */
        Immediate,
        /**
         * @brief The presentation engine waits for vertical sync before swapping/presenting textures.
        */
        Fifo,
        /**
         * @brief The presentation engine waits for vertical sync before swapping textures internally, but enables swapping between the textures in queue that are waiting to be presented, resulting in no application wait time.
        */
        Mailbox,
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = Mailbox
    };

    /**
     * @brief Converts a present_mode_ext to a string.
     * @return The enum value as a string, or "Invalid present_mode_ext value" if the value was not recognized as an enum member.
    */
    inline std::string to_string(present_mode_ext mode);

    /**
     * @brief Describes the SurfaceEXT's capabilities for SwapchainEXT creation.
     *
     * SwapchainEXT **must** be created with this structure in mind, usually only a limited set of capabilities are available (e.g. swapchain texture formats).
    */
    struct surface_capabilities_ext
    {
        /**
         * @brief The minimum number of textures (inclusive).
        */
        uint32_t minTextureCount;
        /**
         * @brief The maximum number of textures (inclusive).
        */
        uint32_t maxTextureCount;

        /**
         * @brief The minimum texture extent (inclusive).
        */
        extent_2d minExtent;
        /**
         * @brief The maximum texture extent (inclusive).
        */
        extent_2d maxExtent;

        /**
         * @brief The supported texture formats.
        */
        std::vector<format> formats;

        /**
         * @brief The supported swapchain present modes.
        */
        std::vector<present_mode_ext> presentModes;

        /**
         * @brief The supported texture usage flags bits
        */
        resource_usage_flags usageBits;
    };
}
