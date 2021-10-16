/**
 * @file fence.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    /**
     * @brief Fence flag bits describe how the fence should be created.
    */
    enum struct fence_flag_bits : uint32_t
    {
        /**
         * @brief A default fence.
        */
        None = 0,
        /**
         * @brief The fence starts out as signaled, meaning it can be waited upon.
         * This can be useful for handling the first frame in frame loops that expect the fence to be signaled from the previous frame.
        */
        Signaled = 1 << 0,
    };
    LLRI_DEFINE_FLAG_BIT_OPERATORS(fence_flag_bits);

    /**
     * @brief Converts a fence_flag_bits to a string.
     * @return The enum value as a string, or "Invalid fence_flag_bits value" if the value was not recognized as an enum member.
    */
    std::string to_string(fence_flag_bits bits);

    /**
     * @brief Fence flags describe how the fence should be created.
     * fence_flags are created by combining one or more fence_flag_bits.
    */
    using fence_flags = flags<fence_flag_bits>;

    /**
     * @brief Converts fence_flags to a string.
     * @return The flags as a string, or "Invalid fence_flags value" if the value was not recognized as a valid combination of fence_flag_bits
    */
    std::string to_string(fence_flags flags);

    /**
     * @brief Fence is a synchronization structure that enables synchronization between GPU and CPU events.
     * Fences are often signaled by Queues after submitting CommandLists, after which the Fence can be waited upon using Device::waitFences().
    */
    class Fence
    {
        friend class Device;
        friend class Queue;

    private:
        // Force private constructor/deconstructor so that only create/destroy can manage lifetime
        Fence() = default;
        ~Fence() = default;

        void* m_ptr = nullptr;
        void* m_event = nullptr;
        uint64_t m_counter = 0;
        bool m_signaled = false;
    };
}
