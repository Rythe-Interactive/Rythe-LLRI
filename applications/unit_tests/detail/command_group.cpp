/**
 * @file command_group.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <doctest/doctest.h>
#include <unit_tests/helpers.hpp>

TEST_CASE("CommandGroup")
{
    auto* instance = helpers::defaultInstance();
    auto* adapter = helpers::selectAdapter(instance);
    auto* device = helpers::defaultDevice(instance, adapter);
    auto* group = helpers::defaultCommandGroup(device, helpers::availableQueueType(adapter), 10);
    
    SUBCASE("CommandGroup::reset()")
    {
        SUBCASE("[Correct usage] Empty CommandGroup")
        {
            CHECK_EQ(group->reset(), llri::result::Success);
        }

        SUBCASE("[Incorrect usage] CommandList still recording")
        {
            llri::CommandList* cmdList;
            REQUIRE_EQ(group->allocate(llri::command_list_alloc_desc { llri::command_list_usage::Direct }, &cmdList), llri::result::Success);
            REQUIRE_EQ(cmdList->begin(llri::command_list_begin_desc { }), llri::result::Success);

            CHECK_EQ(group->reset(), llri::result::ErrorInvalidState);

            REQUIRE_EQ(cmdList->end(), llri::result::Success);
        }
    }

    SUBCASE("CommandGroup::allocate() (single)")
    {
    }

    SUBCASE("CommandGroup::allocate() (multi)")
    {
        
    }

    SUBCASE("CommandGroup::free() (single)")
    {
        
    }

    SUBCASE("CommandGroup::free() (multi)")
    {
        
    }

    device->destroyCommandGroup(group);
    instance->destroyDevice(device);
    llri::destroyInstance(instance);
}
