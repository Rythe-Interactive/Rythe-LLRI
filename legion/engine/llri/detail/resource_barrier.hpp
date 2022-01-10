/**
 * @file resource_barrier.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    class Resource;

    /**
     * @brief The type of resource_barrier - this type is used to determine which value to pick from the anonymous resource_barrier union.
     */
    enum struct resource_barrier_type
    {
        /**
         * @brief Use the resource_barrier_transition struct.
         */
        ReadWrite,
        /**
         * @brief Use the resource_barrier_read_write struct.
         */
        Transition,
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = Transition
    };

    /**
     * @brief Converts a resource_barrier_type to a string.
     * @return The enum value as a string, or "Invalid resource_barrier_type value" if the value was not recognized as an enum member.
    */
    std::string to_string(resource_barrier_type type)
    {
        switch(type)
        {
            case resource_barrier_type::ReadWrite:
                return "ReadWrite";
            case resource_barrier_type::Transition:
                return "Transition";
            default:
                break;
        }
        
        return "Invalid resource_barrier_type value";
    }

    /**
     * @brief Transitions a resource from one state to another. Operations and memory dependencies on the resource are handled properly according to the transition.
     */
    struct resource_barrier_transition
    {
        Resource* resource;
    };

    /**
     * @brief All ReadWrite writing operations on this Resource must complete before any future ReadWrite operations can begin.
     *
     * You do not need to insert a read_write barrier between two passes if the resource is only being read to in both passes, nor do you need an additional read_write barrier if the resource is already being transitioned to/from a ReadWrite state using resource_barrier_transition.
     */
    struct resource_barrier_read_write
    {
        /**
         * @brief The resource that the barrier applies to.
         *
         * @note Valid usage (ErrorInvaliUsage): **Must** be a valid non-null pointer to a resource object.
         */
        Resource* resource;
    };

    /**
     * @brief Describes a memory dependency.
     */
    struct resource_barrier
    {
        resource_barrier_type type;
        
        union {
            resource_barrier_read_write rw;
            resource_barrier_transition trans;
        };
        
        resource_barrier read_write(Resource* resource)
        {
            resource_barrier barrier;
            barrier.type = resource_barrier_type::ReadWrite;
            barrier.rw = resource_barrier_read_write { resource };
            return barrier;
        }
        
        resource_barrier transition(Resource* resource)
        {
            resource_barrier barrier {};
            barrier.type = resource_barrier_type::Transition;
            barrier.trans = resource_barrier_transition { resource };
            return barrier;
        }
    };
}
