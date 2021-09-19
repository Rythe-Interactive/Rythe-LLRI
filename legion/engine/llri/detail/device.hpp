/**
 * @file device.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
//detail includes should be kept to a minimum but
//are allowed as long as dependencies are upwards (e.g. device may include adapter but not vice versa)
#include <llri/detail/flags.hpp>
#include <llri/detail/adapter.hpp>

namespace LLRI_NAMESPACE
{
    struct adapter_extension;
    struct queue_desc;
    enum struct queue_type : uint8_t;
    class Queue;

    struct command_group_desc;
    class CommandGroup;

    enum struct fence_flag_bits : uint32_t;
    using fence_flags = flags<fence_flag_bits>;
    class Fence;

    /**
     * @brief Device description to be used in Instance::createDevice().
    */
    struct device_desc
    {
        /**
         * @brief The adapter to create the instance for.
        */
        Adapter* adapter;
        /**
         * @brief The enabled adapter features.
         * It is **recommended** to only enable features that will be used because unused enabled features might disable driver optimizations.
        */
        adapter_features features;

        /**
         * @brief The number of device extensions in the device_desc::extensions array.
        */
        uint32_t numExtensions;
        /**
         * @brief The adapter extensions, if device_desc::numExtensions > 0, then this **must** be a valid pointer to an array of adapter_extension.
         * If device_desc::numExtensions == 0, then this pointer **may** be nullptr.
        */
        adapter_extension* extensions;

        /**
         * @brief The number of queues that are in the device_desc::queues array. Device **can not** be created without queues, thus this value **must** be at least 1 or higher.
        */
        uint32_t numQueues;
        /**
         * @brief An array of device queue descriptions, which is used to create the queues upon device creation. This value **must** be a valid pointer to an array of queue_desc structures, with a size of at least device_desc::numQueues.
        */
        queue_desc* queues;
    };

    /**
     * @brief A Device is a virtual representation of an Adapter and can create/destroy/allocate/query resources for the said Adapter.
     */
    class Device
    {
        friend Instance;
        friend class CommandGroup;
    public:
        /**
         * @brief Query a created Queue by type and index.
         *
         * All queues are created upon device creation, and stored for quick access through queryQueue(). Queues are thus owned by the Device, the user **may** query the created queues for use, but the user never obtains ownership over the queue.
         *
         * Queues are stored contiguously (but separated by type) in the order of device_desc::queues. Thus if device_desc::queues contained [Graphics, Compute, Graphics, Transfer, Graphics], the graphics queues for that array could be accessed with index 0, 1, 2, and not by their direct index in the array.
         *
         * @param type The type of Queue. This value must be a valid queue_type value, and at least one of this queue type must have been requested during device creation.
         * @param index The Queue array index. Queues are stored per type so this index must be from 0 to n-1 where n is the number of requested queues of this particular type.
         * @param queue A pointer to the resulting queue variable.
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if type is not a valid enum value
         * @return ErrorInvalidUsage if index is more than the number of queues created of the given type
         * @return ErrorInvalidUsage if queue is nullptr.
         *
         * @note (Device nodes) Queues are shared across device nodes. The API selects nodes (Adapters) to execute the commands on based on command list parameters.
        */
        result queryQueue(queue_type type, uint8_t index, Queue** queue);

        /**
        * @brief Get the number of created queues of a given type.
        */
        uint8_t queryQueueCount(queue_type type);

        /**
         * @brief Create a command group. Command groups are responsible for allocating and managing the necessary device memory for command queues.
         *
         * @param desc The description of the command group.
         * @param cmdGroup A pointer to the resulting command group variable.
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if cmdGroup is nullptr.
         * @return ErrorInvalidUsage if desc.type is not a valid queue_type enum value.
         * @return ErrorInvalidUsage if desc.count is 0.
         * @return ErrorInvalidUsage if this device's Device::queryQueueCount(desc.type) returns 0.
         * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory.
        */
        result createCommandGroup(const command_group_desc& desc, CommandGroup** cmdGroup);

        /**
         * @brief Destroy the command group.
         * @param cmdGroup A pointer to a valid CommandGroup, or nullptr.
        */
        void destroyCommandGroup(CommandGroup* cmdGroup);

        /**
         * @brief Create a Fence which can be used for cpu-gpu synchronization.
         * @param flags Flags to describe how the Fence should be created.
         * @param fence A pointer to the resulting fence variable.
         * @return Success upon correct execution of the operation.
         *
         * @return ErrorInvalidUsage if fence is nullptr.
         * @return ErrorInvalidUsage if flags is not a valid combination of fence_flags enum values.
         * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory.
        */
        result createFence(fence_flags flags, Fence** fence);

        /**
         * @brief Destroy the Fence.
         * @param fence A pointer to a valid Fence, or nullptr.
        */
        void destroyFence(Fence* fence);

        /**
         * @brief Wait for each fence in the array to reach their signal, or until the timeout value.
         *
         * If all fences have already reached their signal, this function returns immediately.
         *
         * If any of the fences have not reached their signal, the function will block until all fences do. If the timeout occurs before all of the fences reach their signal, the operation returns result::Timeout, and none of the fences are reset.
         *
         * @param numFences The number of fences in the fences array.
         * @param fences An array of Fence pointers. Each fence must be a valid pointer to a Fence.
         * @param timeout Timeout is the time in milliseconds until the function **must** return. If timeout is more than 0, the function will block as described above. If timeout is 0, then no blocking occurs, but the function returns Success if all fences reach their signal, and returns Timeout if (some of) fences did not.
         *
         * @return Success upon correct execution of the operation, if all fences finish within the timeout.
         * @return Timeout if the wait time for the fences was longer than their wait time.
         * @return ErrorInvalidUsage if numFences was 0.
         * @return ErrorInvalidUsage if fences was nullptr.
         * @return ErrorInvalidUsage if any of the Fence pointers in the fences array were nullptr.
         * @return ErrorNotSignaled if any of the fences have not been signaled and thus can never reach their signal.
         * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory, ErrorDeviceLost.
        */
        result waitFences(uint32_t numFences, Fence** fences, uint64_t timeout);

        /**
         * @brief Utility function. Equivalent of calling waitFences(1, &fence, timeout). Refer to the documentation of waitFences() for information on its usage.
         * @return All possible result values from Device::waitFences().
        */
        result waitFence(Fence* fence, uint64_t timeout);

    private:
        //Force private constructor/deconstructor so that only create/destroy can manage lifetime
        Device() = default;
        ~Device() = default;

        void* m_ptr = nullptr;
        Adapter* m_adapter = nullptr;
        void* m_functionTable = nullptr;

        validation_callback_desc m_validationCallback;
        void* m_validationCallbackMessenger = nullptr;

        std::vector<Queue*> m_graphicsQueues;
        std::vector<Queue*> m_computeQueues;
        std::vector<Queue*> m_transferQueues;

        result impl_createCommandGroup(const command_group_desc& desc, CommandGroup** cmdGroup);
        void impl_destroyCommandGroup(CommandGroup* cmdGroup);

        result impl_createFence(fence_flags flags, Fence** fence);
        void impl_destroyFence(Fence* fence);
        result impl_waitFences(uint32_t numFences, Fence** fences, uint64_t timeout);
    };
}
