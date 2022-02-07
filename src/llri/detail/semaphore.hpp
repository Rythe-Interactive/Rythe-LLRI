/**
 * @file semaphore.hpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    /**
     * @brief Semaphore is a synchronization structure that enables synchronization between GPU events.
     * Semaphores are signaled by Queue and Swapchain, after which Queue can wait on them, enabling GPU event synchronization without CPU interference.
    */
    class Semaphore
    {
        friend class Device;
        friend class Queue;

    public:
        using native_semaphore = void;

        /**
         * @brief Gets the native Semaphore  pointer, which depending on the llri::getImplementation() is a pointer to the following:
         *
         * DirectX12: ID3D12Fence*
         * Vulkan: VkSemaphore
         */
        [[nodiscard]] native_semaphore* getNative() const
        {
            return m_ptr;
        }
        
    private:
        // Force private constructor/deconstructor so that only create/destroy can manage lifetime
        Semaphore() = default;
        ~Semaphore() = default;

        native_semaphore* m_ptr = nullptr;
        uint64_t m_counter = 0;
    };
}
