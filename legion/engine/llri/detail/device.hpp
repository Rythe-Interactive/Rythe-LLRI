/**
 * @file device.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
//detail includes should be kept to a minimum but
//are allowed as long as dependencies are upwards (e.g. device may include adapter but not vice versa)
#include <llri/detail/adapter.hpp>

namespace LLRI_NAMESPACE
{
    struct adapter_extension;
    struct queue_desc;
    enum struct queue_type : uint8_t;
    class Queue;

    struct command_group_desc;
    class CommandGroup;

    struct command_list_alloc_desc;
    class CommandList;

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
         * @brief Create a command group. Command groups are responsible for allocating and managing the necessary device memory for command queues.
         *
         * @param desc The description of the command group.
         * @param cmdGroup A pointer to the resulting command group variable.
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if cmdGroup is nullptr.
         * @return ErrorInvalidUsage if desc.type is not a valid queue_type enum value.
         * @return ErrorInvalidUsage if desc.count is 0.
         * @return ErrorInvalidUsage if this device's Adapter::queryQueueCount(desc.type) returns 0.
         * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory.
        */
        result createCommandGroup(const command_group_desc& desc, CommandGroup** cmdGroup) const;

        /**
         * @brief Destroy the command group.
         * @param cmdGroup A pointer to a valid CommandGroup, or nullptr.
        */
        void destroyCommandGroup(CommandGroup* cmdGroup) const;

        /**
         * @brief Allocate a CommandList from the CommandGroup in command_list_alloc_desc. The resulting CommandList will be of the same queue_type as the the CommandGroup.
         *
         * Allocated CommandLists **may** be free'd from the CommandGroup through Device::freeCommandList() or Device::freeCommandLists(), but if that isn't done, the CommandGroup will free all of its CommandLists upon destruction.
         *
         * @param desc The allocation description describes the command list's usage.
         * @param cmdList A pointer to the resulting command list variable;
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if cmdList was nullptr.
         * @return ErrorInvalidUsage if desc.group was nullptr.
         * @return ErrorInvalidUsage if desc.usage was not a valid command_list_usage enum value.
         * @return ErrorExceededLimit if the CommandGroup doesn't have enough CommandLists to allocate another.
         * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory.
        */
        result allocateCommandList(const command_list_alloc_desc& desc, CommandList** cmdList) const;

        /**
         * @brief Allocate one or more CommandLists from the CommandGroup in command_list_alloc_desc. The resulting CommandLists will be of the same queue_type as the CommandGroup.
         *
         * Allocated CommandLists **may** be free'd from the CommandGroup through Device::freeCommandList() or Device::freeCommandLists(), but if that isn't done, the CommandGroup will free all of its CommandLists upon destruction.
         *
         * @param desc The allocation description describes the command list's usage. Each CommandList is allocated with this same structure.
         * @param count The number of CommandLists to allocate.
         * @param cmdLists A pointer to the resulting command list vector.
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if cmdLists was nullptr.
         * @return ErrorInvalidUsage if desc.group was nullptr.
         * @return ErrorInvalidUsage if desc.usage was not a valid command_list_usage enumv alue.
         * @return ErrorInvalidUsage if count was 0.
         * @return ErrorExceededLimit if the CommandGroup doesn't have enough CommandLists to allocate count CommandLists.
         * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory.
        */
        result allocateCommandLists(const command_list_alloc_desc& desc, uint8_t count, std::vector<CommandList*>* cmdLists) const;

        /**
         * @brief Free the CommandList from the CommandGroup, which allows the CommandGroup to release all of the device memory allocated for the CommandList.
         *
         * @param group The CommandGroup to free the CommandList from.
         * @param cmdList The CommandList to be free'd and destroyed.
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if group is nullptr.
         * @return ErrorInvalidUsage if cmdList is nullptr.
         * @return ErrorInvalidUsage if the cmdList didn't belong to the CommandGroup.
        */
        result freeCommandList(CommandGroup* group, CommandList* cmdList) const;

        /**
         * @brief Free the CommandLists from the CommandGroup, which allows the CommandGroup to release all of the device memory allocated for the CommandLists.
         *
         * @param group The CommandGroup to free the CommandLists from.
         * @param numCommandLists The number of CommandLists to be free'd, *should** match the size of the cmdLists array.
         * @param cmdLists An array of CommandGroups, **must** not be nullptr and **must** at least contain numCommandLists amount of CommandLists.
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if group is nullptr.
         * @return ErrorInvalidUsage if numCommandLists is 0.
         * @return ErrorInvalidUsage if cmdLists is nullptr.
         * @return ErrorInvalidUsage if any of the CommandLists in cmdLists is nullptr.
         * @return ErrorInvalidUsage if any of the CommandLists in cmdLists doesn't belong to the CommandGroup.
        */
        result freeCommandLists(CommandGroup* group, uint8_t numCommandLists, CommandList** cmdLists) const;

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

        result impl_createCommandGroup(const command_group_desc& desc, CommandGroup** cmdGroup) const;
        void impl_destroyCommandGroup(CommandGroup* cmdGroup) const;

        result impl_allocateCommandList(const command_list_alloc_desc& desc, CommandList** cmdList) const;
        result impl_allocateCommandLists(const command_list_alloc_desc& desc, uint8_t count, std::vector<CommandList*>* cmdLists) const;
        result impl_freeCommandList(CommandGroup* group, CommandList* cmdList) const;
        result impl_freeCommandLists(CommandGroup* group, uint8_t numCommandLists, CommandList** cmdLists) const;
    };
}
