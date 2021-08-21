/**
 * @file command_group.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
//detail includes should be kept to a minimum but
//are allowed as long as dependencies are upwards (e.g. adapter may include instance but not vice versa)
#include <llri/detail/instance.hpp>
#include <cstdint>
#include <list>

namespace LLRI_NAMESPACE
{
    enum struct result : uint8_t;
    class CommandList;
    enum struct queue_type : uint8_t;

    /**
     * @brief Describes how the CommandGroup should be created.
    */
    struct command_group_desc
    {
        /**
         * @brief The type of queue that this CommandGroup allocates for. A CommandList allocated through CommandGroup must only submit to queues of this type.
        */
        queue_type type;
        /**
         * @brief The maximum number of CommandLists in the group. Must be more than 0.
        */
        uint8_t count;
    };

    /**
     * @brief CommandGroups are responsible for allocating the memory required to record CommandLists. They **may** be used to allocate one or multiple CommandLists but never more than command_group_desc::count.
     *
     * @note CommandGroups are not thread-safe. CommandLists allocated through the same CommandGroup **can not** be recorded from separate threads simultaneously. For multi-threaded recording, create at least one separate CommandGroup per thread to prevent this from becoming an issue.
    */
    class CommandGroup
    {
        friend class Device;

    public:
        /**
         * @brief Reset the CommandGroup and all of the allocated CommandLists.
         * After this, the CommandLists in this CommandGroup will be ready for recording again.
         *
         * @note The CommandLists in this CommandGroup **can not** be in use in Queue::executeCommandLists() at this time.
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidState One of the CommandLists in the group is currently in the command_list_state::Recording state.
         * @return Implementation defined result values: ErrorOutOfDeviceMemory.
        */
        result reset();

    private:
        //Force private constructor/deconstructor so that only create/destroy can manage lifetime
        CommandGroup() = default;
        ~CommandGroup() = default;

        void* m_ptr = nullptr;
        void* m_deviceHandle = nullptr;
        void* m_deviceFunctionTable = nullptr;
        std::list<CommandList*> m_cmdLists;
        validation_callback_desc m_validationCallback;
        void* m_validationCallbackMessenger = nullptr;

        queue_type m_type;
        uint8_t m_maxCount;

        result impl_reset();
    };
}
