/**
 * @file command_list.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <unit_tests/helpers.hpp>
#include <doctest/doctest.h>

TEST_CASE("CommandList")
{
    auto* instance = helpers::defaultInstance();
    auto* adapter = helpers::selectAdapter(instance);
    auto* device = helpers::defaultDevice(instance, adapter);
    auto* group = helpers::defaultCommandGroup(device, helpers::availableQueueType(adapter), 10);
    auto* list = helpers::defaultCommandList(group, llri::command_list_usage::Direct);

    SUBCASE("CommandList::begin()")
    {
        SUBCASE("[Correct usage] Valid parameters and CommandList is empty")
        {
            llri::command_list_begin_desc desc{};
            CHECK_EQ(list->begin(desc), llri::result::Success);
            REQUIRE_EQ(list->end(), llri::result::Success);
        }

        SUBCASE("[Incorrect usage] CommandList was already recording")
        {
            llri::command_list_begin_desc desc{};
            CHECK_EQ(list->begin(desc), llri::result::Success);
            //already recording
            CHECK_EQ(list->begin(desc), llri::result::ErrorInvalidState);
            REQUIRE_EQ(list->end(), llri::result::Success);
        }

        SUBCASE("[Incorrect usage] CommandList has previously been recorded")
        {
            llri::command_list_begin_desc desc{};
            CHECK_EQ(list->begin(desc), llri::result::Success);
            REQUIRE_EQ(list->end(), llri::result::Success);

            //previously recorded
            CHECK_EQ(list->begin(desc), llri::result::ErrorInvalidState);
        }

        SUBCASE("[Incorrect usage] The CommandGroup this CommandList belongs to was already recording a CommandList")
        {
            auto* list2 = helpers::defaultCommandList(group, llri::command_list_usage::Direct);

            REQUIRE_EQ(list->begin({}), llri::result::Success);

            CHECK_EQ(list2->begin({}), llri::result::ErrorOccupied);

            REQUIRE_EQ(list->end(), llri::result::Success);
        }
    }

    SUBCASE("CommandList::end()")
    {
        SUBCASE("[Correct usage] CommandList is recording")
        {
            llri::command_list_begin_desc desc{};
            REQUIRE_EQ(list->begin(desc), llri::result::Success);
            CHECK_EQ(list->end(), llri::result::Success);
        }

        SUBCASE("[Incorrect usage] CommandList wasn't recording")
        {
            // Two possibilities, list may be empty:
            CHECK_EQ(list->end(), llri::result::ErrorInvalidState);

            // Or list may have already recorded
            llri::command_list_begin_desc desc{};
            REQUIRE_EQ(list->begin(desc), llri::result::Success);
            REQUIRE_EQ(list->end(), llri::result::Success);
            CHECK_EQ(list->end(), llri::result::ErrorInvalidState);
        }
    }
}
