/**
 * @file resource_barrier.hpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    class Resource;

    /**
     * @brief The type of resource_barrier - this type is used to determine which value to pick from the anonymous resource_barrier union.
     */
    enum struct resource_barrier_type : uint8_t
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
    inline std::string to_string(resource_barrier_type type)
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
        /**
         * @brief The resource that needs to be transitioned.
         *
         * @note Valid usage (ErrorInvalidUsage): **Must** be a valid non-null pointer to a resource object.
        */
        Resource* resource;
        /**
         * @brief The state that the resource is transitioning from.
         *
         * @note Valid usage: oldState **must** match the resource's current state or in the case of subresource_range transitions it **must** match with  the resource's current state for every subresource in the transition.
         */
        resource_state oldState;
        /**
         * @brief The state to transition to.
         *
         * @note Valid usage (ErrorInvalidUsage): newState **must not** be more than resource_state::MaxEnum.
         * @note Valid usage (ErrorInvalidState): the conditions described in the resource_state **must** be met.
         * @note Valid usage (ErrorInvalidUsage): newState **must not** be the same as oldState.
        */
        resource_state newState;
        
        /**
         * @brief The range of subresources to transition if the resource is a resource_type::Texture1D, Texture2D, or Texture3D.
         *
         * @note Ignored if resource is resource_type::Buffer.
         * @note Valid usage (ErrorInvalidUsage): the conditions in texture_subresource_range **must** be met.
         */
        texture_subresource_range subresourceRange;
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
         * @note Valid usage (ErrorInvalidUsage): **Must** be a valid non-null pointer to a resource object.
         * @note Valid usage (ErrorInvalidState): **Must** be in the resource_state::ReadWrite state.
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
        
        static resource_barrier read_write(Resource* resource)
        {
            resource_barrier barrier {};
            barrier.type = resource_barrier_type::ReadWrite;
            barrier.rw = resource_barrier_read_write { resource };
            return barrier;
        }
        
        static resource_barrier transition(Resource* resource, resource_state oldState, resource_state newState, texture_subresource_range range = texture_subresource_range::all())
        {
            resource_barrier barrier {};
            barrier.type = resource_barrier_type::Transition;
            barrier.trans = resource_barrier_transition { resource, oldState, newState, range };
            return barrier;
        }
    };
}
