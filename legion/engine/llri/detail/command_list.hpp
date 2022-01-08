/**
 * @file command_list.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    class CommandGroup;

    /**
     * @brief Describes how the CommandList is going to be used. A CommandList's usage is exclusive and can not be changed after allocation.
    */
    enum struct command_list_usage : uint8_t
    {
        /**
         * @brief The CommandList will be directly submitted to a Queue.
        */
        Direct,
        /**
         * @brief The CommandList is indirect and can be submitted to another CommandList before that CommandList is submitted to a queue.
         * This can be useful for recording a set of commands that can be dynamically submitted to other lists, which might save CPU time.
         *
         * Indirect CommandLists **can not** be submitted to a queue directly.
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
    inline std::string to_string(command_list_usage usage);

    /**
     * @brief Describes how the command list(s) should be allocated what their usage will be.
    */
    struct command_list_alloc_desc
    {
        /**
         * @brief The Device node on which the command list(s) should be allocated. Every bit in the mask corresponds to a node (if present) and for command list allocation a single bit **must** be set.
         *
         * For convenience, 0 **may** be passed, which is interpreted as 1 and refers to the first/default node.
         *
         * @note Valid usage (ErrorInvalidNodeMask): The node mask **must not** have a bit set if the bit is at a position more than or equals Adapter::queryNodeCount().
         * @note Valid usage (ErrorInvalidNodeMask): The node mask **must not** have more than one bit set to positive.
        */
        uint32_t nodeMask;

        /**
         * @brief Describes if the CommandList will be used to be submitted to Queues directly or if they will be submitted indirectly through other CommandLists.
         *
         * @note Valid usage (ErrorInvalidUsage): The usage must be less or equal to command_list_usage::MaxEnum.
        */
        command_list_usage usage;
    };
    
    /**
     * @brief The current state of the CommandList.
     *
     * The CommandList's state defines which operations can be applied to it. 
    */
    enum struct command_list_state : uint8_t
    {
        /**
         * @brief The default state of a CommandList. A CommandList is in this state just after creation, or after having been reset through CommandGroup::reset(). From here, CommandList **may** transition into command_list_state::Recording through CommandList::begin().
        */
        Empty,
        /**
         * @brief When a CommandList is in this state, commands may be submitted to the CommandList, but it **can not** be used for any other operations. Once CommandList::end() is called, it is transitioned into the command_list_state::Ready state.
        */
        Recording,
        /**
         * @brief A CommandList in this state **can** be submitted through Queue::submit(). CommandLists in this state **can not** be opened for recording again but instead **must** be reset through CommandGroup::reset() before they **can** be used for recording again.
        */
        Ready
    };

    /**
     * @brief Contextual information about how (and on what GPU) the CommandList will be submitted, and what kind of information was previously set (if this is an indirect CommandList).
    */
    struct command_list_begin_desc
    {
        // Empty placeholder structure for future begin information
    };

    /**
     * @brief Converts a command_list_state to a string.
     * @return The enum value as a string, or "Invalid command_list_state value" if the value was not recognized as an enum member.
    */
    inline std::string to_string(command_list_state state);

    class CommandList
    {
        friend class Device;
        friend class CommandGroup;
        friend class Queue;

    public:
        /**
         * Get the desc that the CommandList was allocated with.
         */
        [[nodiscard]] command_list_alloc_desc getDesc() const;

        /**
         * @brief Set the CommandList in a command_list_state::Recording state.
         *
         * The CommandList **must** be in the command_list_state::Empty state for it to transition into a command_list_state::Recording state.
         * If the CommandList is in the command_list_state::Ready state, it **must** first be reset to command_list_state::Empty through CommandGroup::reset() before calling this function.
         *
         * @param desc Contains contextual information about how (and on what GPU) the CommandList will be submitted, and what kind of information was previously set if this is an indirect CommandList.
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidState if the CommandList was not in the command_list_state::Empty state.
         *
         * @note The memory required for CommandList recording is allocated through its CommandGroup. Because of this, commandLists allocated through the same CommandGroup **can not** be recorded simultaneously and are thus not thread-safe. For multi-threaded recording, it is recommended to create at least one separate CommandGroup per thread to prevent this from becoming an issue.
        */
        result begin(const command_list_begin_desc& desc);

        /**
         * @brief Set the CommandList in the command_list_state::Ready state.
         *
         * The CommandList **must** be in the command_list_state::Recording state for it to transition into a command_list_state::Ready state.
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidState if the CommandList was not in the command_list_state::Recording state.
        */
        result end();

        /**
         * @brief Shorthand convenience function for recording the CommandList.
         * This is the equivalent of calling CommandList::begin(), running the commands in the passed function, and then calling CommandList::end().
         *
         * Prior to this function call, CommandList must be in the command_list_state::Empty state. Afterwards it will be in the command_list_state::Ready state.
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
         * After creation, CommandList will initially be in the command_list_state::Empty state. Once CommandList::begin() is called, the CommandList will be in the command_list_state::Recording state. After that, when CommandList::end() is called, CommandList will be put in the command_list_state::Ready state, in which it will stay until CommandGroup::reset() is called.
        */
        [[nodiscard]] command_list_state getState() const { return m_state; }
    private:
        // Force private constructor/deconstructor so that only alloc/free can manage lifetime
        CommandList() = default;
        ~CommandList() = default;

        void* m_ptr = nullptr;
        CommandGroup* m_group = nullptr;

        void* m_deviceHandle = nullptr;
        void* m_deviceFunctionTable = nullptr;

        command_list_alloc_desc m_desc;
        command_list_state m_state = command_list_state::Empty;

        void* m_validationCallbackMessenger = nullptr;

        result impl_begin(const command_list_begin_desc& desc);
        result impl_end();
    };
}
