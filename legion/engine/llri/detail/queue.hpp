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
    constexpr const char* to_string(queue_priority priority);

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
    constexpr const char* to_string(queue_type type);

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
     * @brief Queues are used to send commands to the Adapter. This is done by submitting CommandLists and/or synchronization operations.
    */
    class Queue
    {
        friend class Instance;
        friend class Device;

    public:

    private:
        //Force private constructor/deconstructor so that only create/destroy can manage lifetime
        Queue() = default;
        ~Queue() = default;

        std::vector<void*> m_ptrs;
		
        void* m_deviceHandle = nullptr;
        void* m_deviceFunctionTable = nullptr;

        validation_callback_desc m_validationCallback;
        void* m_validationCallbackMessenger = nullptr;
    };
}
