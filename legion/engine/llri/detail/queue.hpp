/**
 * @file queue.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <cstdint>
#include <vector>

//detail includes should be kept to a minimum but
//are allowed as long as dependencies are upwards (e.g. adapter may include instance but not vice versa)
#include <llri/detail/instance.hpp>

namespace LLRI_NAMESPACE
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
     * Queues are never created manually and can only be created through device_desc, after which they **may** be queried through Device::queryQueue().
    */
    struct queue_desc
    {
        /**
         * @brief The type of queue. This determines the set of commands that the queue can support, and the optimizations it **may** enable for that set of commands.
        */
        queue_type type;
        /**
         * @brief The priority of the queue. This **may** affect which queues receive more or less adapter computing resources.
        */
        queue_priority priority;
    };

    /**
     * @brief Describes how command lists should be submitted to a queue.
    */
    struct submit_desc
    {
        /**
         * @brief The nodeMask determines which adapter node the queues are submitted to. A single bit **must** be set.
         *
         * All the CommandLists **must** be created for this nodemask.
        */
        uint32_t nodeMask;

        /**
         * @brief The number of CommandLists in submit_desc::commandLists. **Must** not be 0.
        */
        uint32_t numCommandLists;
        /**
         * @brief An array of CommandList pointers that ought to be submitted to the queue.
         * This array **must not** be nullptr, **must** be of at least size numCommandLists, and all pointers in the array **must** be valid CommandList pointers.
        */
        CommandList** commandLists;

        /**
         * @brief The number of Semaphores in submit_desc::waitSemaphores. **May** be 0.
        */
        uint32_t numWaitSemaphores;
        /**
         * @brief An array of Semaphores that the CommandLists must wait on prior to executing.
         *
         * This array **may** be nullptr if numWaitSemaphores is 0.
         * If not, then this array **must not** be nullptr, the array **must** be of at least size numWaitSemaphores, and all pointers in the array **must** be valid Semaphore pointers.
        */
        Semaphore** waitSemaphores;

        /**
         * @brief The number of Semaphores in submit_desc::signalSemaphores. **May** be 0.
        */
        uint32_t numSignalSemaphores;
        /**
         * @brief An array of Semaphores that the CommandList signal after they are done executing.
         *
         * This array **may** be nullptr if numSignalSemaphores is 0.
         * If not, then this array **must not** be nullptr, the array **must** be of at least size numSignalSemaphores, and all pointers in the array **must** be valid Semaphore pointers.
        */
        Semaphore** signalSemaphores;

        /**
         * @brief A fence to signal after the CommandLists are done executing. **Must** be a valid pointer to a Fence, or nullptr.
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

    public:
        /**
         * @brief Submit CommandLists to the queue, which means the commands they contain will be executed.
         * @param desc Describes the CommandLists that get executed, and what synchronization they signal or wait upon.
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidNodeMask if desc.nodeMask had a bit set to 1 which was not represented by a node in the Device. The nodeMask must always have its positive bit set at Adapter::queryNodeCount() or less.
         * @return ErrorIncompatibleNodeMask if any of the CommandLists in desc.commandLists was not created with the same nodeMask as desc.nodeMask.
         *
         * @return ErrorInvalidUsage if desc.numCommandLists is 0.
         * @return ErrorInvalidUsage if desc.commandLists is nullptr.
         * @return ErrorInvalidUsage if any of the CommandLists in desc.commandLists is nullptr.
         *
         * @return ErrorInvalidState if any of the CommandLists in desc.commandLists was not in the command_list_state::Ready state.
         *
         * @return ErrorInvalidUsage if desc.numWaitSemaphores is more than 0 and desc.waitSemaphores is nullptr.
         * @return ErrorInvalidUsage if desc.numWaitSemaphores is more than 0 and any of the pointers in desc.waitSemaphores is nullptr.
         *
         * @return ErrorInvalidUsage if desc.numSignalSemaphores is more than 0 and desc.signalSemaphores is nullptr.
         * @return ErrorInvalidUsage if desc.numSignalSemaphores is more than 0 and any of the pointers in desc.signalSemaphores is nullptr.
         *
         * @return ErrorAlreadySignaled if desc.fence has already been signaled and has not yet been waited upon.
        */
        result submit(const submit_desc& desc);

        /**
         * @brief Wait for the queue to go idle. This function blocks the CPU thread until all of the commands on the queue are done.
         *
         * This is the equivalent of adding a fence to every submit and waiting for those fences.
         *
         * @return Success upon correct execution of the operation.
         * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory, ErrorDeviceLost.
        */
        result waitIdle();
    private:
        //Force private constructor/deconstructor so that only create/destroy can manage lifetime
        Queue() = default;
        ~Queue() = default;

        std::vector<void*> m_ptrs;
        std::vector<Fence*> m_fences; // optional internal fences for waitIdle()

        Device* m_device = nullptr;

        void* m_validationCallbackMessenger = nullptr;

        result impl_submit(const submit_desc& desc);
        result impl_waitIdle();
    };
}
