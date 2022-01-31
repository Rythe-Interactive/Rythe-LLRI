/**
 * @file swapchain_ext.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    /**
     * @brief Describes how the presentation engine interacts with a swapchain's buffers and the surface.
    */
    enum struct present_mode : uint8_t
    {
        /**
         * @brief The presentation engine doesn't wait for vertical sync, which may result in visible tearing.
        */
        Immediate,
        /**
         * @brief The presentation engine waits for vertical sync before swapping/presenting buffers.
        */
        Fifo,
        /**
         * @brief The presentation engine waits for vertical sync before swapping buffers internally, but enables swapping between the buffers in queue that are waiting to be presented, resulting in no application wait time.
        */
        Mailbox,
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = Mailbox
    };

    /**
     * @brief Converts a present_mode to a string.
     * @return The enum value as a string, or "Invalid present_mode value" if the value was not recognized as an enum member.
    */
    inline std::string to_string(present_mode mode);
}
