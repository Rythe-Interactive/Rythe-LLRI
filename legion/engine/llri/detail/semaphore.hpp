/**
 * @file semaphore.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <cstdint>

namespace LLRI_NAMESPACE
{
    /**
     * @brief Semaphore is a synchronization structure that enables synchronization between GPU events.
     * Semaphores are signaled by Queue and Swapchain, after which Queue can wait on them, enabling GPU event synchronization without CPU interference.
    */
    class Semaphore
    {
        friend class Device;

    private:
        //Force private constructor/deconstructor so that only create/destroy can manage lifetime
        Semaphore() = default;
        ~Semaphore() = default;

        void* m_ptr = nullptr;
        uint64_t m_counter = 0;
    };
}
