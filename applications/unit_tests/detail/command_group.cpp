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
        SUBCASE("[Correct usage] Valid parameters")
        {
            llri::CommandList* cmdList;
            llri::command_list_alloc_desc desc { llri::command_list_usage::Direct };
            CHECK_EQ(group->allocate(desc, &cmdList), llri::result::Success);
        }

        SUBCASE("[Incorrect usage] Invalid command_list_usage enum value")
        {
            llri::CommandList* cmdList;
            llri::command_list_alloc_desc desc { static_cast<llri::command_list_usage>(UINT_MAX) };
            CHECK_EQ(group->allocate(desc, &cmdList), llri::result::ErrorInvalidUsage);
        }

        SUBCASE("[Incorrect usage] More CommandLists than available")
        {
            auto* smallGroup = helpers::defaultCommandGroup(device, helpers::availableQueueType(adapter), 1);

            llri::CommandList* cmdList;
            llri::command_list_alloc_desc desc { llri::command_list_usage::Direct };
            REQUIRE_EQ(smallGroup->allocate(desc, &cmdList), llri::result::Success);

            llri::CommandList* cmdList2;
            CHECK_EQ(smallGroup->allocate(desc, &cmdList2), llri::result::ErrorExceededLimit);

            device->destroyCommandGroup(smallGroup);
        }
    }

    SUBCASE("CommandGroup::allocate() (multi)")
    {
        SUBCASE("[Correct usage] Valid parameters")
        {
            std::vector<llri::CommandList*> cmdLists;
            llri::command_list_alloc_desc desc { llri::command_list_usage::Direct };
            CHECK_EQ(group->allocate(desc, 5, &cmdLists), llri::result::Success);
        }

        SUBCASE("[Incorrect usage] Invalid command_list_usage enum value")
        {
            std::vector<llri::CommandList*> cmdLists;
            llri::command_list_alloc_desc desc { static_cast<llri::command_list_usage>(UINT_MAX) };
            CHECK_EQ(group->allocate(desc, 1, &cmdLists), llri::result::ErrorInvalidUsage);
        }

        SUBCASE("[Incorrect usage] More CommandLists than available")
        {
            auto* smallGroup = helpers::defaultCommandGroup(device, helpers::availableQueueType(adapter), 5);

            std::vector<llri::CommandList*> cmdLists;
            llri::command_list_alloc_desc desc { llri::command_list_usage::Direct };
            REQUIRE_EQ(smallGroup->allocate(desc, 6, &cmdLists), llri::result::ErrorExceededLimit);

            device->destroyCommandGroup(smallGroup);
        }
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
