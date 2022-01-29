/**
 * @file commands.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <helpers.hpp>
#include <doctest/doctest.h>

#include <detail/commands/resource_barrier.hpp>

TEST_CASE("CommandList:: commands")
{
    auto* instance = helpers::defaultInstance();
    auto* adapter = helpers::selectAdapter(instance);
    auto* device = helpers::defaultDevice(instance, adapter);
    auto* group = helpers::defaultCommandGroup(device, helpers::availableQueueType(adapter));
    auto* list = helpers::defaultCommandList(group, 0, llri::command_list_usage::Direct);

    SUBCASE("resourceBarrier()")
        testCommandListResourceBarrier(device, group, list);
    
    device->destroyCommandGroup(group);
    instance->destroyDevice(device);
    llri::destroyInstance(instance);
}