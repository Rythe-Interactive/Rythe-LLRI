/**
 * @file command_list.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
//detail includes should be kept to a minimum but
//are allowed as long as dependencies are upwards (e.g. adapter may include instance but not vice versa)
#include <llri/detail/instance.hpp>
#include <cstdint>

namespace LLRI_NAMESPACE
{
    class CommandGroup;

    /**
     * @brief Describes how the CommandList is going to be used. Each usage is exclusive from one another and can not be changed.
    */
    enum struct command_list_usage : uint8_t
    {
        /**
         * @brief The CommandList will be directly submitted to a Queue.
        */
        Direct,
        /**
         * @brief The CommandList is indirect and will be submitted to another CommandList before that CommandList is submitted to a queue.
        */
        Indirect,
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = Indirect
    };

    /**
     * @brief Converts a command_list_usage to a string.
     * @return The enum value as a string, or "Invalid command_list_usage value" if the value was not recognized as an enum member.
    */
    constexpr const char* to_string(command_list_usage usage);

    /**
     * @brief Describes how many CommandLists should be allocated through the given CommandGroup, and what their usage will be.
    */
    struct command_list_alloc_desc
    {
        /**
         * @brief The CommandGroup to allocate the list(s) from.
         *
         * This CommandGroup **must** have enough unused CommandList slots left to fit command_list_alloc_desc::count new CommandLists.
        */
        CommandGroup* group;
        /**
         * @brief Describes if the CommandLists will be used to be submitted to Queues directly or if they will be submitted indirectly through other CommandLists.
        */
        command_list_usage usage;
        /**
         * @brief The number of CommandLists to allocate. This must not exceed the number of free CommandLists in command_list_alloc_desc::group.
        */
        uint32_t count;
    };
    
    /**
     * @brief The current state of the CommandList.
     *
     * The CommandList's state defines which operations can be applied to it. 
    */
    enum struct command_list_state : uint8_t
    {
        /**
         * @brief The default state of a CommandList. A CommandList is in this state just after creation, or after having been reset through CommandGroup::reset(). From here, CommandList may transition into command_list_state::Recording through CommandList::begin().
        */
        Empty,
        /**
         * @brief When a CommandList is in this state, commands may be submitted to the CommandList, but it may not be used for any other operations yet. Once CommandList::end() is called, it is transitioned into the command_list_state::Executable state.
        */
        Recording,
        /**
         * @brief A CommandList in this state can be executed through Queue::executeCommandLists(). CommandLists in this state can not be opened for recording again but instead must be reset through CommandGroup::reset() before use.
        */
        Executable
    };

    /**
     * @brief Contextual information about how (and on what GPU) the CommandList will be executed, and what kind of information was previously set (if this is an indirect CommandList).
    */
    struct command_list_begin_desc
    {
        //TODO
    };

    /**
     * @brief Converts a command_list_state to a string.
     * @return The enum value as a string, or "Invalid command_list_state value" if the value was not recognized as an enum member.
    */
    constexpr const char* to_string(command_list_state state);

    class CommandList
    {
        friend class Device;
        friend class CommandGroup;

    public:
        /**
         * @brief Set the CommandList in a command_list_state::Recording state.
         *
         * The CommandList **must** be in the command_list_state::Empty state for it to transition into a command_list_state::Recording state.
         * If the CommandList is in the command_list_state::Executable state, it **must** first be reset to command_list_state::Empty through CommandGroup::reset() before calling this function.
         *
         * @param desc Contains contextual information about how (and on what GPU) the CommandList will be executed, and what kind of information was previously set if this is an indirect CommandList.
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidState if the CommandList was not in the command_list_state::Empty state.
         *
         * @note The memory required for CommandList recording is allocated through its CommandGroup. Because of this, commandLists allocated through the same CommandGroup **can not** be recorded simultaneously and are thus not thread-safe. For multi-threaded recording, create at least one separate CommandGroup per thread to prevent this from becoming an issue.
        */
        result begin(const command_list_begin_desc& desc);

        /**
         * @brief Set the CommandList in the command_list_state::Executable state.
         *
         * The CommandList **must** be in the command_list_state::Recording state for it to transition into a command_list_state::Executable state.
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidState if the CommandList was not in the command_list_state::Recording state.
        */
        result end();

        /**
         * @brief Shorthand convenience function for recording the CommandList.
         * This is the equivalent of calling CommandList::begin(), running the commands in the passed function, and then calling CommandList::end().
         *
         * Prior to this function call, CommandList must be in the command_list_state::Empty state. Afterwards it will be in the command_list_state::Executable state.
         *
         * @note Read the documentation for CommandList::begin() and CommandList::end() for information on their valid usage, return values, etc.
         *
         * @param desc Contains contextual information about how (and on what GPU) the CommandList will be executed, and what kind of information the CommandList inherits if this is an indirect CommandList.
         * @param function A function (usually a lambda), with Args as parameters. Within the scope of the function, CommandList commands **can** be recorded.
         * @param args The parameter values to be passed into function.
         *
         * @return Success upon correct execution of the operation.
         * @return Any errors listed in CommandList::begin() and CommandList::end().
        */
        template<typename Func, typename ...Args>
        result record(const command_list_begin_desc& desc, Func&& function, Args&&... args);

        /**
         * @brief Returns the current state of the CommandList.
         *
         * After creation, CommandList will initially be in the command_list_state::Empty state. Once CommandList::begin() is called, the CommandList will be in the command_list_state::Recording state. After that, when CommandList::end() is called, CommandList will be put in the command_list_state::Executable state, in which it will stay until CommandGroup::reset() is called.
        */
        [[nodiscard]] command_list_state queryState() const { return m_state; }
    private:
        //Force private constructor/deconstructor so that only create/destroy can manage lifetime
        CommandList() = default;
        ~CommandList() = default;

        void* m_ptr = nullptr;
        command_list_state m_state = command_list_state::Empty;

        validation_callback_desc m_validationCallback;
        void* m_validationCallbackMessenger = nullptr;

        result impl_begin(const command_list_begin_desc& desc);
        result impl_end();
    };
}
