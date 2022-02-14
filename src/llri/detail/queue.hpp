/**
 * @file queue.hpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    class CommandList;
    class Fence;
    class Semaphore;

    /**
     * @brief Declare queue priority. Queues with a higher priority **may** be assigned more resources and processing time by the Adapter.
    */
    enum struct queue_priority : uint8_t
    {
        /**
         * @brief Normal priority. Queues with this priority **may** be superseded in processing time by queues created with queue_priority::High.
         *
         * Normal priority only applies to in-application queue sorting and has no effect on system-wide queue sorting.
        */
        Normal,
        /**
         * @brief High priority. Queues with this priority **may** receive more processing time than queues created with queue_priority::Normal.
         *
         * High priority only applies to in-application queue sorting and has no effect on system-wide queue sorting.
        */
        High,
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = High
    };

    /**
     * @brief Converts a queue_priority to a string.
     * @return The enum value as a string, or "Invalid queue_priority value" if the value was not recognized as an enum member.
    */
    inline std::string to_string(queue_priority priority);

    /**
     * @brief Queue types define what kind of commands can be sent to the queue created with it.
     * Some queue types support multiple types of commands, whereas others support only one or two types, which **might** enable additional performance optimizations for the said commands.
    */
    enum struct queue_type : uint8_t
    {
        /**
         * @brief Graphics queues support all commands related to graphics, compute, and transfer operations.
         *
         * Graphics queues tend to be the most general purpose, but is most commonly available.
        */
        Graphics,
        /**
         * @brief Compute queues support all commands related to compute and transfer operations.
         *
         * Compute **may** use different Adapter resources than graphics commands, so sending compute commands through a Compute queue might help them be sent through to the Adapter sooner without interfering with Graphics commands.
        */
        Compute,
        /**
         * @brief Transfer queues only support transfer operations.
         *
         * Transfer queues **may** be optimized for PCIe operations, which **may** allow them to make better use of the PCIe lanes' speeds.
        */
        Transfer,
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = Transfer
    };

    /**
     * @brief Converts a queue_type to a string.
     * @return The enum value as a string, or "Invalid queue_type value" if the value was not recognized as an enum member.
    */
    inline std::string to_string(queue_type type);

    /**
     * @brief Describes the information needed to create a queue upon device creation.
     *
     * Queues are never created manually and can only be created through device_desc, after which they **may** be queried through Device::getQueue().
    */
    struct queue_desc
    {
        /**
         * @brief The type of queue. This determines the set of commands that the queue can support, and the optimizations it **may** enable for that set of commands.
         *
         * @note Valid usage (ErrorInvalidUsage):  type must not be more than queue_type::MaxEnum.
        */
        queue_type type;
        /**
         * @brief The priority of the queue. This **may** affect which queues receive more or less adapter computing resources.
         *
         * @note Valid usage (ErrorInvalidUsage):  priority must not be more than queue_priority::MaxEnum.
        */
        queue_priority priority;
    };

    /**
     * @brief Describes how command lists should be submitted to a queue.
    */
    struct submit_desc
    {
        /**
         * @brief The nodeMask determines which adapter node the queues are submitted to. If this value is 0, it is interpreted as 1.
         *
         * @note Valid usage (ErrorInvalidNodeMask): Exactly one bit **must** be set, and that bit **must** be less than 1 << Adapter::queryNodeCount().
         * @note Valid usage (ErrorIncompatibleNodeMask):  All the CommandLists in this submit **must** have been allocated with this nodemask.
        */
        uint32_t nodeMask;

        /**
         * @brief The number of CommandLists in the submit_desc::commandLists array.
         *
         * @note Valid usage (ErrorInvalidUsage): numCommandLists **must** be more than 0.
        */
        uint32_t numCommandLists;
        /**
         * @brief An array of CommandList pointers (of size numCommandLists) that ought to be submitted to the queue.
         *
         * @note Valid usage (ErrorInvalidUsage): commandLists **must** be a valid non-null pointer to a CommandList* array.
         * @note Valid usage (ErrorInvalidUsage): Each element in commandLists **must** be a valid non-null CommandList.
         * @note Valid usage (ErrorInvalidState): Each commandlist in the array **must** be in the command_list_state::Ready state.
        */
        CommandList** commandLists;

        /**
         * @brief The number of Semaphores in the submit_desc::waitSemaphores array.
        */
        uint32_t numWaitSemaphores;
        /**
         * @brief An array of Semaphores that the CommandLists must wait on prior to executing.
         *
         * @note Valid usage: if numWaitSemaphores == 0 then waitSemaphores **may** be nullptr.
         * @note Valid usage (ErrorInvalidUsage): if numWaitSemaphores > 0 then waitSemaphores **must** be a valid non-null pointer to an array of size numWaitSemaphores (or more).
         * @note Valid usage (ErrorInvalidUsage): if numWaitSemaphores > 0 then each element in waitSemaphores **must** be a valid non-null Semaphore pointer.
        */
        Semaphore** waitSemaphores;

        /**
         * @brief The number of Semaphores in the submit_desc::signalSemaphores array.
        */
        uint32_t numSignalSemaphores;
        /**
         * @brief An array of Semaphores that the CommandList signal after they are done executing.
         *
         * @note Valid usage: if numSignalSemaphores == 0 then signalSemaphores **may** be nullptr.
         * @note Valid usage (ErrorInvalidUsage): if numSignalSemaphores > 0 then signalSemaphores **must** be a valid non-null pointer to an array of size numSignalSemaphores (or more).
         * @note Valid usage (ErrorInvalidUsage): if numSignalSemaphores > 0 then each element in signalSemaphores **must** be a valid non-null Semaphore pointer.
        */
        Semaphore** signalSemaphores;

        /**
         * @brief A fence to signal after the CommandLists are done executing.
         *
         * @note Valid usage: fence may be a valid pointer to a Fence, or nullptr.
         * @Note Valid usage (ErrorAlreadySignaled): if fence is not nullptr, then the Fence **must not** have already been signaled.
        */
        Fence* fence;
    };

    /**
     * @brief Queues are used to send commands to the Adapter. This is done by submitting CommandLists and/or synchronization operations.
    */
    class Queue
    {
        friend class Instance;
        friend class Device;
        friend class SwapchainEXT;

    public:
        using native_queue = void;

        /**
         * Get the desc that the Queue has been created with (internally).
         */
        [[nodiscard]] queue_desc getDesc() const;

        /**
         * @brief Gets the native Queue pointer, which depending on the llri::getImplementation() is a pointer to the following:
         *
         * DirectX12: ID3D12CommandQueue*
         * Vulkan: VkQueue
         *
         * @param index The index of the device node to get the queue of. The function returns nullptr if the index exceeds the number of nodes in the device.
         */
        [[nodiscard]] native_queue* getNative(size_t index = 0) const;
        
        /**
         * @brief Submit CommandLists to the queue, which means the commands they contain will be executed.
         * @param desc Describes the CommandLists that get executed, and what synchronization they signal or wait upon.
         *
         * @return Success upon correct execution of the operation.
         * @return submit_desc defined result values: ErrorInvalidUsage, ErrorInvalidNodeMask, ErrorIncompatibleNodeMask, ErrorInvalidState, ErrorAlreadySignaled.
        */
        result submit(const submit_desc& desc);

        /**
         * @brief Wait for the queue to go idle. This function blocks the CPU thread until all of the commands on the queue are done.
         *
         * This is the equivalent of adding a fence to the last submit and waiting for the said fence.
         *
         * @return Success upon correct execution of the operation.
         * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory, ErrorDeviceLost.
        */
        result waitIdle();
    private:
        // Force private constructor/deconstructor so that only create/destroy can manage lifetime
        Queue() = default;
        ~Queue() = default;

        std::vector<native_queue*> m_ptrs;
        std::vector<Fence*> m_fences; // optional internal fences for waitIdle()

        queue_desc m_desc;
        Device* m_device = nullptr;

        void* m_validationCallbackMessenger = nullptr;

        result impl_submit(const submit_desc& desc);
        result impl_waitIdle();
    };
}
