/**
 * @file fence.hpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
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
    LLRI_DEFINE_FLAG_BIT_OPERATORS(fence_flag_bits)

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

    public:
        using native_fence = void;

        /**
         * Get the flags that the Fence was created with.
         */
        [[nodiscard]] fence_flags getFlags() const;

        /**
         * @brief Gets the native Fence pointer, which depending on the llri::getImplementation() is a pointer to the following:
         *
         * DirectX12: ID3D12Fence*
         * Vulkan: VkFence
         */
        [[nodiscard]] native_fence* getNative() const;
    private:
        // Force private constructor/deconstructor so that only create/destroy can manage lifetime
        Fence() = default;
        ~Fence() = default;

        fence_flags m_flags;

        native_fence* m_ptr = nullptr;
        void* m_event = nullptr;
        uint64_t m_counter = 0;
        bool m_signaled = false;
    };
}
