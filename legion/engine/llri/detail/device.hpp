/**
 * @file device.hpp
 * Copyright (c) 2021 Leon Brands
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    enum struct adapter_extension : uint8_t;
    struct queue_desc;
    enum struct queue_type : uint8_t;
    class Queue;

    class CommandGroup;

    enum struct fence_flag_bits : uint32_t;
    using fence_flags = flags<fence_flag_bits>;
    class Fence;

    class Semaphore;

    class Resource;
    struct resource_desc;

    /**
     * @brief Device description to be used in Instance::createDevice().
    */
    struct device_desc
    {
        /**
         * @brief The adapter to create the instance for.
         *
         * @note Valid usage (ErrorInvalidUsage): adapter **must** be a valid non-null pointer.
        */
        Adapter* adapter;
        /**
         * @brief The enabled adapter features.
         * It is **recommended** to only enable features that will be used because unused enabled features might disable driver optimizations.
         *
         * @note Valid usage (ErrorFeatureNotSupported): All enabled features **must** be enabled in Adapter::queryFeatures().
        */
        adapter_features features;

        /**
         * @brief The number of device extensions in the device_desc::extensions array.
         *
         * @note Valid usage (ErrorExceededLimit):  As device_desc::extensions can only hold unique values, numExtensions can not be more than adapter_extension::MaxEnum + 1.
        */
        uint32_t numExtensions;
        /**
         * @brief An array of adapter extensions, [extensions, extensions + numExtensions - 1].
         *
         * @note Valid usage (ErrorInvalidUsage):  if device_desc::numExtensions > 0, then this **must** be a valid pointer to an array of adapter_extension. If device_desc::numExtensions == 0, then this pointer **may** be nullptr.
         * @note Valid usage (ErrorInvalidUsage): Each value in this array **must** be <= adapter_extension::MaxEnum.
         * @note Valid usage (ErrorInvalidUsage): Each value in this array **must** be unique. // TODO: Enforce this
         * @note Valid usage (ErrorExtensionNotSupported): Adapter::queryExtensionSupport() must return true on each value in this array.
        */
        adapter_extension* extensions;

        /**
         * @brief The number of queues that are in the device_desc::queues array.
         *
         * @note Valid usage (ErrorInvalidUsage): Device **can not** be created without queues, thus this value **must** be at least 1 or higher.
        */
        uint32_t numQueues;
        /**
         * @brief An array of device queue descriptions, which is used to create the queues upon device creation, [queues, queues + numQueues - 1].
         *
         * @note Valid usage (ErrorInvalidUsage):This value **must** be a valid non-null pointer to an array of queue_desc structures, with a size of at least device_desc::numQueues.
         * @note Valid usage (ErrorExceededLimit): The total number of elements with queue_desc::type as a given queue_type **must not** exceed Adapter::queryQueueCount() with that queue_type.
         * @note All conditions in queue_desc must be met.
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
        friend class Queue;
  
    public:
        using native_device = void;

        /**
         * @brief Get the desc that the Device was created with.
         */
        [[nodiscard]] device_desc getDesc() const;
        
        /**
         * @brief Gets the native Device pointer, which depending on the llri::getImplementation() is a pointer to the following:
         *
         * DirectX12: ID3D12Device*
         * Vulkan: VkDevice
         */
        [[nodiscard]] native_device* getNative() const;
        
        /**
         * @brief Get the adapter that the device represents.
         */
        [[nodiscard]] Adapter* getAdapter() const;

        /**
         * @brief Get a created Queue by type and index.
         *
         * All queues are created upon device creation, and stored for quick access through getQueue(). Queues are thus owned by the Device, the user **may** query the created queues for use, but the user never obtains ownership over the queue.
         *
         * Queues are stored contiguously (but separated by type) in the order of device_desc::queues. Thus if device_desc::queues contained [Graphics, Compute, Graphics, Transfer, Graphics], the graphics queues for that array could be accessed with index 0, 1, 2, and not by their direct index in the array.
         *
         * @note (Device nodes) Queues are shared across device nodes. The API selects nodes (Adapters) to execute the commands on based on command list parameters.
         *
         * @param type The type of Queue. This value **must** be a valid queue_type value, and at least one of this queue type **must** have been requested during device creation.
         * @param index The Queue array index. Queues are stored per type so this index **must** be from 0 to n-1 where n is the number of requested queues of this particular type.
         *
         * @return The requested Queue if all conditions were met, or nullptr otherwise.
        */
        Queue* getQueue(queue_type type, uint8_t index);

        /**
         * @brief Get the number of created queues of a given type.
        */
        uint8_t queryQueueCount(queue_type type);

        /**
         * @brief Create a command group. Command groups are responsible for allocating and managing the necessary device memory for command queues.
         *
         * @param type The type of queue that this CommandGroup allocates for. A CommandList allocated through CommandGroup must only submit to queues of this type.
         * @param cmdGroup A pointer to the resulting command group variable.
         *
         * @note Valid usage (ErrorInvalidUsage): cmdGroup **must** be a valid non-null pointer to a CommandGroup* variable.
         * @note Valid usage (ErrorInvalidUsage): type **must** be less or equal to queue_type::MaxEnum.
         * @note Valid usage (ErrorInvalidUsage): Device::queryQueueCount(type) must return more than 0.
         *
         * @return Success upon correct execution of the operation.
         * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory.
        */
        result createCommandGroup(queue_type type, CommandGroup** cmdGroup);

        /**
         * @brief Destroy the command group object.
         *
         * CommandLists allocated through the CommandGroup do **not** have to be be freed manually, but users should note that their handles willl become invalid after this, thus they may no longer be used and they should also not be in use by the GPU at the time of destruction.
         *
         * @param cmdGroup A pointer to a valid CommandGroup, or nullptr.
        */
        void destroyCommandGroup(CommandGroup* cmdGroup);

        /**
         * @brief Create a Fence which can be used for cpu-gpu synchronization.
         *
         * @param flags Flags to describe how the Fence should be created.
         * @param fence A pointer to the resulting fence variable.
         *
         * @note Valid usage (ErrorInvalidUsage): fence **must** be a valid non-null pointer to a Fence* variable.
         * @note Valid usage (ErrorInvalidUsage): flags **must** be a valid combination of fence_flag_bits enum values.
         *
         * @return Success upon correct execution of the operation.
         * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory.
        */
        result createFence(fence_flags flags, Fence** fence);

        /**
         * @brief Destroy the Fence object.
         * @param fence A pointer to a valid Fence, or nullptr.
        */
        void destroyFence(Fence* fence);

        /**
         * @brief Wait for each fence in the array to reach their signal, or until the timeout value.
         *
         * If all fences have already reached their signal, this function returns immediately.
         * If any of the fences have not reached their signal, the function will block until all fences do.
         * If the timeout occurs before all of the fences reach their signal, the operation returns result::Timeout, and none of the fences are reset.
         *
         * When waitFences() returns result::Success, all fences are reset, meaning that they're no longer signaled.
         *
         * @param numFences The number of fences in the fences array.
         * @param fences An array of Fence pointers. Each fence must be a valid pointer to a Fence.
         * @param timeout Timeout is the time in milliseconds until the function **must** return. If timeout is more than 0, the function will block as described above. If timeout is 0, then no blocking occurs, but the function returns Success if all fences reach their signal, and returns Timeout if (some of) fences did not.
         *
         * @note Valid usage (ErrorInvalidUsage): numFences **must** be more than 0.
         * @note Valid usage (ErrorInvalidUsage): fences **must** be a valid non-null pointer to a Fence* array.
         * @note Valid usage (ErrorInvalidUsage): each element in the fences array **must** be a valid non-null pointer to a Fence*.
         * @note Valid usage (ErrorNotSignaled): each fence must have been signaled prior to this call.
         *
         * @return Success upon correct execution of the operation, if all fences finish within the timeout.
         * @return Timeout if the wait time for the fences was longer than their wait time.
         * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory, ErrorDeviceLost.
        */
        result waitFences(uint32_t numFences, Fence** fences, uint64_t timeout);

        /**
         * @brief Utility function. Equivalent of calling waitFences(1, &fence, timeout). Refer to the documentation of waitFences() for information on its usage.
         * @return All possible result values from Device::waitFences().
        */
        result waitFence(Fence* fence, uint64_t timeout);

        /**
         * @brief Create a Semaphore, which can be used for synchronization between GPU events.
         * @param semaphore A pointer to the resulting Semaphore variable.
         *
         * @note Valid uage (ErrorInvalidUsage): semaphore **must** be a valid non-null pointer to a Semaphore* variable.
         *
         * @return Success upon correct execution of the operation.
         * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory.
        */
        result createSemaphore(Semaphore** semaphore);

        /**
         * @brief Destroy the given Semaphore object.
         * @param semaphore A pointer to a valid Semaphore, or nullptr.
        */
        void destroySemaphore(Semaphore* semaphore);

        /**
         * @brief Create a resource (a buffer or texture) and allocate the memory for it.
         * @param desc The description of the resource.
         * @param resource A pointer to the resulting resource variable.
         *
         * @note Valid usage (ErrorInvalidUsage): resource must be a valid non-null pointer to a Resource* variable.
         *
         * @return Success upon correct execution of the operation.
         * @return resource_desc defined result values: ErrorInvalidUsage, ErrorInvalidNodeMask.
         * @return ErrorOutOfDeviceMemory implementations may return this if the resource does not fit in the Device's memory.
        */
        result createResource(const resource_desc& desc, Resource** resource);

        /**
         * @brief Destroy the given resource.
         * @param resource A pointer to a valid Resource, or nullptr.
        */
        void destroyResource(Resource* resource);
    private:
        // Force private constructor/deconstructor so that only create/destroy can manage lifetime
        Device() = default;
        ~Device() = default;

        native_device* m_ptr = nullptr;
        Adapter* m_adapter = nullptr;
        void* m_functionTable = nullptr;

        void* m_validationCallbackMessenger = nullptr;

        std::vector<Queue*> m_graphicsQueues;
        std::vector<Queue*> m_computeQueues;
        std::vector<Queue*> m_transferQueues;

        device_desc m_desc;
        
        // used for internal commands/work (e.g. transitioning internal states)
        void* m_workCmdGroup = nullptr;
        void* m_workCmdList = nullptr;
        void* m_workFence = nullptr;
        queue_type m_workQueueType;

        result impl_createCommandGroup(queue_type type, CommandGroup** cmdGroup);
        void impl_destroyCommandGroup(CommandGroup* cmdGroup);

        result impl_createFence(fence_flags flags, Fence** fence);
        void impl_destroyFence(Fence* fence);
        result impl_waitFences(uint32_t numFences, Fence** fences, uint64_t timeout);

        result impl_createSemaphore(Semaphore** semaphore);
        void impl_destroySemaphore(Semaphore* semaphore);

        result impl_createResource(const resource_desc& desc, Resource** resource);
        void impl_destroyResource(Resource* resource);
    };
}
