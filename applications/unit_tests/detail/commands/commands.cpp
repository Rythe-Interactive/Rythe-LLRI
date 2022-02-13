/**
 * @file commands.cpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#include <llri/llri.hpp>
#include <helpers.hpp>
#include <doctest/doctest.h>

#include <detail/commands/resource_barrier.hpp>

TEST_CASE("CommandList:: commands")
{
    auto* instance = detail::defaultInstance();
    
    detail::iterateAdapters(instance, [=](llri::Adapter* adapter) {
        auto* device = detail::defaultDevice(instance, adapter);
        auto* group = detail::defaultCommandGroup(device, detail::availableQueueType(adapter));
        auto* list = detail::defaultCommandList(group, 0, llri::command_list_usage::Direct);

        SUBCASE("resourceBarrier()")
            testCommandListResourceBarrier(device, group, list);
        
        device->destroyCommandGroup(group);
        instance->destroyDevice(device);
    });

    llri::destroyInstance(instance);
}
