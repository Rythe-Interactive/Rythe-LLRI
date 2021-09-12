/**
 * @file command_group.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <array>
#include <doctest/doctest.h>
#include <helpers.hpp>

TEST_CASE("CommandGroup")
{
    auto* instance = helpers::defaultInstance();
    auto* adapter = helpers::selectAdapter(instance);
    auto* device = helpers::defaultDevice(instance, adapter);
    auto* group = helpers::defaultCommandGroup(device, helpers::availableQueueType(adapter), 10);

    const auto nodeCount = adapter->queryNodeCount();
    for (uint32_t i = 0; i < nodeCount; i++)
    {
        uint32_t nodeMask = 1 << i;
        const std::string str = std::string("Device node ") + std::to_string(nodeMask);
        SUBCASE(str.c_str())
        {
            SUBCASE("CommandGroup::reset()")
            {
                SUBCASE("[Correct usage] Empty CommandGroup")
                {
                    CHECK_EQ(group->reset(), llri::result::Success);
                }

                SUBCASE("[Incorrect usage] CommandList still recording")
                {
                    llri::CommandList* cmdList;
                    REQUIRE_EQ(group->allocate(llri::command_list_alloc_desc { nodeMask, llri::command_list_usage::Direct }, &cmdList), llri::result::Success);
                    REQUIRE_EQ(cmdList->begin(llri::command_list_begin_desc { }), llri::result::Success);

                    CHECK_EQ(group->reset(), llri::result::ErrorInvalidState);

                    REQUIRE_EQ(cmdList->end(), llri::result::Success);
                }

                SUBCASE("[Correct usage] Valid usage")
                {
                    std::vector<llri::CommandList*> cmdLists;
                    REQUIRE_EQ(group->allocate(llri::command_list_alloc_desc{ nodeMask, llri::command_list_usage::Direct }, 5, &cmdLists), llri::result::Success);

                    CHECK_EQ(group->reset(), llri::result::Success);
                }
            }

            SUBCASE("CommandGroup::allocate() (single)")
            {
                SUBCASE("[Correct usage] Valid parameters")
                {
                    for (uint8_t usage = 0; usage <= static_cast<uint8_t>(llri::command_list_usage::MaxEnum); usage++)
                    {
                        llri::CommandList* cmdList;
                        llri::command_list_alloc_desc desc{ nodeMask, static_cast<llri::command_list_usage>(usage) };
                        CHECK_EQ(group->allocate(desc, &cmdList), llri::result::Success);
                    }
                }

                SUBCASE("[Incorrect usage] Invalid command_list_usage enum value")
                {
                    llri::CommandList* cmdList;
                    llri::command_list_alloc_desc desc { nodeMask, static_cast<llri::command_list_usage>(UINT_MAX) };
                    CHECK_EQ(group->allocate(desc, &cmdList), llri::result::ErrorInvalidUsage);
                }

                SUBCASE("[Incorrect usage] More CommandLists than available")
                {
                    auto* smallGroup = helpers::defaultCommandGroup(device, helpers::availableQueueType(adapter), 1);

                    llri::CommandList* cmdList;
                    llri::command_list_alloc_desc desc { nodeMask, llri::command_list_usage::Direct };
                    REQUIRE_EQ(smallGroup->allocate(desc, &cmdList), llri::result::Success);

                    llri::CommandList* cmdList2;
                    CHECK_EQ(smallGroup->allocate(desc, &cmdList2), llri::result::ErrorExceededLimit);

                    device->destroyCommandGroup(smallGroup);
                }

                SUBCASE("[Incorrect usage] Nodemask for non-existing node")
                {
                    uint32_t incorrectNodeMask = 1 << nodeCount;
                    llri::CommandList* cmdList;
                    llri::command_list_alloc_desc desc { incorrectNodeMask, llri::command_list_usage::Direct };
                    CHECK_EQ(group->allocate(desc, &cmdList), llri::result::ErrorInvalidNodeMask);
                }

                if (nodeCount > 1)
                {
                    SUBCASE("[Incorrect usage] Nodemask with multiple enabled bits")
                    {
                        constexpr uint32_t incorrectNodeMask = 1 << 0 | 1 << 1;
                        llri::CommandList* cmdList;
                        llri::command_list_alloc_desc desc { incorrectNodeMask, llri::command_list_usage::Direct };
                        CHECK_EQ(group->allocate(desc, &cmdList), llri::result::ErrorInvalidNodeMask);
                    }
                }
            }

            SUBCASE("CommandGroup::allocate() (multi)")
            {
                SUBCASE("[Correct usage] Valid parameters")
                {
                    for (uint8_t usage = 0; usage <= static_cast<uint8_t>(llri::command_list_usage::MaxEnum); usage++)
                    {
                        std::vector<llri::CommandList*> cmdLists;
                        llri::command_list_alloc_desc desc{ nodeMask, static_cast<llri::command_list_usage>(usage) };
                        CHECK_EQ(group->allocate(desc, 2, &cmdLists), llri::result::Success);
                    }
                }

                SUBCASE("[Incorrect usage] Invalid command_list_usage enum value")
                {
                    std::vector<llri::CommandList*> cmdLists;
                    llri::command_list_alloc_desc desc { nodeMask, static_cast<llri::command_list_usage>(UINT_MAX) };
                    CHECK_EQ(group->allocate(desc, 2, &cmdLists), llri::result::ErrorInvalidUsage);
                }

                SUBCASE("[Incorrect usage] More CommandLists than available")
                {
                    auto* smallGroup = helpers::defaultCommandGroup(device, helpers::availableQueueType(adapter), 5);

                    std::vector<llri::CommandList*> cmdLists;
                    llri::command_list_alloc_desc desc { nodeMask, llri::command_list_usage::Direct };
                    REQUIRE_EQ(smallGroup->allocate(desc, 6, &cmdLists), llri::result::ErrorExceededLimit);

                    device->destroyCommandGroup(smallGroup);
                }
            }

            SUBCASE("CommandGroup::free() (single)")
            {
                SUBCASE("[Incorrect usage] CommandList is nullptr")
                {
                    CHECK_EQ(group->free(nullptr), llri::result::ErrorInvalidUsage);
                }

                SUBCASE("[Correct usage] CommandList was created through the group")
                {
                    llri::CommandList* cmdList;
                    REQUIRE_EQ(group->allocate({ nodeMask, llri::command_list_usage::Direct }, &cmdList), llri::result::Success);

                    CHECK_EQ(group->free(cmdList), llri::result::Success);
                }

                SUBCASE("[Incorrect usage] CommandList was currently recording")
                {
                    llri::CommandList* cmdList;
                    REQUIRE_EQ(group->allocate({ nodeMask, llri::command_list_usage::Direct }, &cmdList), llri::result::Success);

                    llri::command_list_begin_desc beginDesc{};
                    REQUIRE_EQ(cmdList->begin(beginDesc), llri::result::Success);

                    CHECK_EQ(group->free(cmdList), llri::result::ErrorInvalidState);

                    REQUIRE_EQ(cmdList->end(), llri::result::Success);
                }

                SUBCASE("[Incorrect usage] CommandList wasn't created through group")
                {
                    llri::CommandGroup* group2 = helpers::defaultCommandGroup(device, helpers::availableQueueType(adapter), 1);
                    llri::CommandList* cmdList;
                    REQUIRE_EQ(group2->allocate({ nodeMask, llri::command_list_usage::Direct }, &cmdList), llri::result::Success);

                    CHECK_EQ(group->free(cmdList), llri::result::ErrorInvalidUsage);
                }
            }

            SUBCASE("CommandGroup::free() (multi)")
            {
                SUBCASE("[Incorrect usage] CommandList is nullptr")
                {
                    CHECK_EQ(group->free(1, nullptr), llri::result::ErrorInvalidUsage);
                }

                SUBCASE("[Incorrect usage] count is 0")
                {
                    std::vector<llri::CommandList*> cmdLists;
                    CHECK_EQ(group->free(0, cmdLists.data()), llri::result::ErrorInvalidUsage);
                }

                SUBCASE("[Correct usage] CommandLists were created through the group")
                {
                    std::vector<llri::CommandList*> cmdLists;
                    REQUIRE_EQ(group->allocate({ nodeMask, llri::command_list_usage::Direct }, 2, &cmdLists), llri::result::Success);

                    CHECK_EQ(group->free(cmdLists.size(), cmdLists.data()), llri::result::Success);
                }

                SUBCASE("[Incorrect usage] Some of the CommandLists weren't created through the group")
                {
                    std::array<llri::CommandList*, 2> cmdLists;

                    auto* group2 = helpers::defaultCommandGroup(device, helpers::availableQueueType(adapter), 1);

                    REQUIRE_EQ(group->allocate({ nodeMask, llri::command_list_usage::Direct }, &cmdLists[0]), llri::result::Success);
                    REQUIRE_EQ(group2->allocate({ nodeMask, llri::command_list_usage::Direct }, &cmdLists[1]), llri::result::Success);

                    CHECK_EQ(group->free(cmdLists.size(), cmdLists.data()), llri::result::ErrorInvalidUsage);

                    device->destroyCommandGroup(group2);
                }

                SUBCASE("[Incorrect usage] None of the CommandLists were created through the group")
                {
                    std::vector<llri::CommandList*> cmdLists;

                    auto* group2 = helpers::defaultCommandGroup(device, helpers::availableQueueType(adapter), 2);
                    REQUIRE_EQ(group2->allocate({ nodeMask, llri::command_list_usage::Direct }, 2, &cmdLists), llri::result::Success);

                    CHECK_EQ(group->free(cmdLists.size(), cmdLists.data()), llri::result::ErrorInvalidUsage);

                    device->destroyCommandGroup(group2);
                }

                SUBCASE("[Incorrect usage] One of the CommandLists was currently recording")
                {
                    std::vector<llri::CommandList*> cmdLists;

                    REQUIRE_EQ(group->allocate({ nodeMask, llri::command_list_usage::Direct }, 2, &cmdLists), llri::result::Success);

                    llri::command_list_begin_desc beginDesc{ };
                    REQUIRE_EQ(cmdLists[0]->begin(beginDesc), llri::result::Success);

                    CHECK_EQ(group->free(cmdLists.size(), cmdLists.data()), llri::result::ErrorInvalidState);

                    REQUIRE_EQ(cmdLists[0]->end(), llri::result::Success);
                }
            }
        }
    }

    device->destroyCommandGroup(group);
    instance->destroyDevice(device);
    llri::destroyInstance(instance);
}
