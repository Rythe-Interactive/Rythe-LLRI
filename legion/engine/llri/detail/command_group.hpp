/**
 * @file command_group.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    enum struct result : uint8_t;
    class CommandList;
    enum struct queue_type : uint8_t;
    struct command_list_alloc_desc;

    /**
     * @brief CommandGroups are responsible for allocating the memory required to record CommandLists. They are used to allocate one or multiple CommandLists.
     *
     * @note CommandGroups are not thread-safe. CommandLists allocated through the same CommandGroup **can not** be recorded from separate threads simultaneously. For multi-threaded recording, it is recommended to create at least one separate CommandGroup per thread to prevent this from becoming an issue.
    */
    class CommandGroup
    {
        friend class Device;
        friend class CommandList;

    public:
        /**
         * Get the type that CommandGroup was created for.
         */
        [[nodiscard]] queue_type getType() const;

        /**
         * @brief Reset the CommandGroup and all of the allocated CommandLists.
         * After this, the CommandLists in this CommandGroup will be ready for recording again.
         *
         * @note The CommandLists in this CommandGroup **can not** be in use in Queue::submit() at this time.
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidState One of the CommandLists in the group is currently in the command_list_state::Recording state.
         * @return Implementation defined result values: ErrorOutOfDeviceMemory.
        */
        result reset();

        /**
         * @brief Allocate a CommandList. The resulting CommandList will be of the same queue_type as the the CommandGroup, and is a non-owning pointer.
         *
         * Allocated CommandLists **may** be free'd through CommandGroup::free(), but if that isn't done, the CommandGroup frees all of its CommandLists upon destruction. CommandGroup maintains ownership over all of its allocated CommandLists.
         *
         * @param desc The allocation description describes the command list's usage.
         * @param cmdList A pointer to the resulting command list variable;
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if cmdList was nullptr.
         * @return ErrorInvalidNodeMask if desc.nodeMask had a bit set which was not represented by a node in the Device. The node mask **must** always have its positive bit set at a position less than Adapter::queryNodeCount().
         * @return ErrorInvalidNodeMask if desc.nodeMask had multiple bits set.
         * @return ErrorInvalidUsage if desc.usage was not a valid command_list_usage enum value.
         * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory.
        */
        result allocate(const command_list_alloc_desc& desc, CommandList** cmdList);

        /**
         * @brief Allocate one or more CommandLists. The resulting CommandLists will be of the same queue_type as the CommandGroup, and are non-owning pointers.
         *
         * Allocated CommandLists **may** be free'd through CommandGroup::free(), but if that isn't done, the CommandGroup will free all of its CommandLists upon destruction. CommandGroup maintains ownership over all of its allocated CommandLists.
         *
         * @param desc The allocation description describes the command list's usage. Each CommandList is allocated with this same structure.
         * @param count The number of CommandLists to allocate.
         * @param cmdLists A pointer to the resulting command list vector.
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if cmdLists was nullptr.
         * @return ErrorInvalidNodeMask if desc.nodeMask had a bit set to 1 which was not represented by a node in the Device. The node mask must always have its positive bit set at Adapter::queryNodeCount() or less.
         * @return ErrorInvalidNodeMask if desc.nodeMask had multiple bits set to 1.
         * @return ErrorInvalidUsage if desc.usage was not a valid command_list_usage enum value.
         * @return ErrorInvalidUsage if count was 0.
         * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory.
        */
        result allocate(const command_list_alloc_desc& desc, uint8_t count, std::vector<CommandList*>* cmdLists);

        /**
         * @brief Free the CommandList, which allows the CommandGroup to release all of the device memory allocated for the CommandList.
         *
         * @param cmdList The CommandList to be free'd and destroyed.
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if cmdList is nullptr.
         * @return ErrorInvalidUsage if the cmdList didn't belong to the CommandGroup.
        */
        result free(CommandList* cmdList);

        /**
         * @brief Free an array of CommandLists, which allows the CommandGroup to release all of the device memory allocated for the CommandLists.
         *
         * @param numCommandLists The number of CommandLists to be free'd, *should** match the size of the cmdLists array.
         * @param cmdLists An array of CommandGroups, **must** not be nullptr and **must** at least contain numCommandLists amount of CommandLists.
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if numCommandLists is 0.
         * @return ErrorInvalidUsage if cmdLists is nullptr.
         * @return ErrorInvalidUsage if any of the CommandLists in cmdLists is nullptr.
         * @return ErrorInvalidUsage if any of the CommandLists in cmdLists doesn't belong to the CommandGroup.
        */
        result free(uint8_t numCommandLists, CommandList** cmdLists);

    private:
        // Force private constructor/deconstructor so that only create/destroy can manage lifetime
        CommandGroup() = default;
        ~CommandGroup() = default;

        void* m_ptr = nullptr;
        void* m_indirectPtr = nullptr;

        Device* m_device = nullptr;
        void* m_deviceFunctionTable = nullptr;

        void* m_validationCallbackMessenger = nullptr;

        queue_type m_type;
        std::unordered_set<CommandList*> m_cmdLists;

#ifndef LLRI_DISABLE_VALIDATION
        CommandList* m_currentlyRecording = nullptr;
#endif

        result impl_reset();

        result impl_allocate(const command_list_alloc_desc& desc, CommandList** cmdList);
        result impl_allocate(const command_list_alloc_desc& desc, uint8_t count, std::vector<CommandList*>* cmdLists);
        result impl_free(CommandList* cmdList);
        result impl_free(uint8_t numCommandLists, CommandList** cmdLists);
    };
}
