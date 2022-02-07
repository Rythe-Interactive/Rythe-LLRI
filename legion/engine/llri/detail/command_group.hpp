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
        using native_command_group = void;
        
        /**
         * Get the type that CommandGroup was created for.
         */
        [[nodiscard]] queue_type getType() const;

        /**
         * @brief Gets the native CommandGroup pointer, which depending on the llri::getImplementation() is a pointer to the following:
         *
         * DirectX12: ID3D12CommandAllocator*
         * Vulkan: VkCommandPool
         */
        [[nodiscard]] native_command_group* getNative() const;
        
        /**
         * @brief Reset the CommandGroup and all of the allocated CommandLists.
         * After this, the CommandLists in this CommandGroup will be ready for recording again.
         *
         * @note The CommandLists in this CommandGroup **can not** be in use in Queue::submit() at this time.
         *
         * @note Valid usage (ErrorInvalidState): None of the CommandLists created by the Group can be in the command_list_state::Recording state.
         *
         * @return Success upon correct execution of the operation.
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
         * @note Valid usage (ErrorInvalidUsage): cmdList **must** be a valid non-null pointer to a CommandList* variable.
         *
         * @return Success upon correct execution of the operation.
         * @return command_list_alloc_desc defined result values: ErrorInvalidNodeMask, ErrorInvalidUsage.
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
         * @note Valid usage (ErrorInvalidUsage): cmdLists **must** be a valid non-null pointer to a std::vector<CommandList*> vector.
         * @note Valid usage (ErrorInvalidUsage): count **must** be more than 0.
         *
         * @return Success upon correct execution of the operation.
         * @return command_list_alloc_desc defined result values: ErrorInvalidNodeMask, ErrorInvalidUsage.
         * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory.
        */
        result allocate(const command_list_alloc_desc& desc, uint8_t count, std::vector<CommandList*>* cmdLists);

        /**
         * @brief Free the CommandList, which allows the CommandGroup to release all of the device memory allocated for the CommandList.
         *
         * @param cmdList The CommandList to be free'd and destroyed.
         *
         * @note Valid usage (ErrorInvalidUsage): The cmdList **must** be a valid non-null pointer to a CommandList.
         * @note Valid usage (ErrorInvalidUsage): The cmdList **must** have been allocated through the CommandGroup.
         *
         * @return Success upon correct execution of the operation.
        */
        result free(CommandList* cmdList);

        /**
         * @brief Free an array of CommandLists, which allows the CommandGroup to release all of the device memory allocated for the CommandLists.
         *
         * @param numCommandLists The number of CommandLists in the cmdLists array.
         * @param cmdLists An array of CommandGroups that ought to be free'd.
         *
         * @note Valid usage (ErrorInvalidUsage): numCommandLists **must** be more than 0.
         * @note Valid usage (ErrorInvalidUsage): cmdLists **must** be a valid non-null pointer to an array of CommandList*s.
         * @note Valid usage (ErrorInvalidUsage): All of the elements in cmdLists **must** be valid non-null pointers.
         * @note Valid usage (ErrorInvalidUsage): All of the elements in cmdLists **must** have been allocated through this CommandGroup.
         *
         * @return Success upon correct execution of the operation.
        */
        result free(uint8_t numCommandLists, CommandList** cmdLists);

    private:
        // Force private constructor/deconstructor so that only create/destroy can manage lifetime
        CommandGroup() = default;
        ~CommandGroup() = default;

        native_command_group* m_ptr = nullptr;
        native_command_group* m_indirectPtr = nullptr;

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
