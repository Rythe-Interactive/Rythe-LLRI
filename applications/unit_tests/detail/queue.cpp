/**
 * @file llri.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <helpers.hpp>
#include <doctest/doctest.h>
#include <array>

struct queue_wrapper
{
    llri::queue_type type;
    uint8_t index;
    llri::Queue* queue;
};

TEST_CASE("Queue")
{
    auto* instance = helpers::defaultInstance();
    auto* adapter = helpers::selectAdapter(instance);
    auto* device = helpers::defaultDevice(instance, adapter);

    // gather all possible queues
    std::vector<queue_wrapper> queues;
    for (uint8_t type = 0; type < static_cast<uint8_t>(llri::queue_type::MaxEnum); type++)
    {
        for (size_t i = 0; i < device->queryQueueCount(static_cast<llri::queue_type>(type)); i++)
        {
            llri::Queue* queue;
            REQUIRE_EQ(device->queryQueue(static_cast<llri::queue_type>(type), i, &queue), llri::result::Success);

            queues.push_back({ static_cast<llri::queue_type>(type), static_cast<uint8_t>(i), queue });
        }
    }

    // queue unit tests must be done for every node to ensure nodemask correctness
    for (size_t node = 0; node < adapter->queryNodeCount(); node++)
    {
        const std::string nodeSubcase = "Device node " + std::to_string(node);
        SUBCASE(nodeSubcase.c_str())
        {
            // and for every queue type available, to ensure queue_type correctness
            uint32_t nodeMask = 1 << node;
            for (auto& wrapper : queues)
            {
                auto* queue = wrapper.queue;
                auto* group = helpers::defaultCommandGroup(device, wrapper.type, 10);

                // premade cmd lists: ready for submit, empty, and recording
                auto* readyCmdList = helpers::defaultCommandList(group, nodeMask, llri::command_list_usage::Direct);
                llri::command_list_begin_desc beginDesc{ };
                REQUIRE_EQ(readyCmdList->begin(beginDesc), llri::result::Success);
                REQUIRE_EQ(readyCmdList->end(), llri::result::Success);

                auto* emptyCmdList = helpers::defaultCommandList(group, nodeMask, llri::command_list_usage::Direct);

                auto* recordingCmdList = helpers::defaultCommandList(group, nodeMask, llri::command_list_usage::Direct);
                REQUIRE_EQ(recordingCmdList->begin(beginDesc), llri::result::Success);

                llri::Fence* signaledFence = helpers::defaultFence(device, true);
                llri::Fence* defaultFence = helpers::defaultFence(device, false);

                const std::string str = to_string(wrapper.type) + " Queue " + std::to_string(wrapper.index);
                SUBCASE(str.c_str())
                {
                    SUBCASE("Queue::submit()")
                    {
                        SUBCASE("[Incorrect usage] incorrect nodemask")
                        {
                            constexpr uint32_t multipleBits = 1 << 0 | 1 << 1; // more than 1 bit set
                            constexpr uint32_t exceedsNodes = UINT_MAX;

                            llri::submit_desc submitDesc{ 0, 1, &readyCmdList, 0, nullptr, 0, nullptr, nullptr };

                            submitDesc.nodeMask = multipleBits;
                            CHECK_EQ(queue->submit(submitDesc), llri::result::ErrorInvalidNodeMask);

                            submitDesc.nodeMask = exceedsNodes;
                            CHECK_EQ(queue->submit(submitDesc), llri::result::ErrorInvalidNodeMask);
                        }

                        if (adapter->queryNodeCount() > 1)
                        {
                            SUBCASE("[Incorrect usage] node mask mismatch between desc.nodeMask and CommandList(s)")
                            {
                                llri::submit_desc submitDesc{ 0, 1, &readyCmdList, 0, nullptr, 0, nullptr, nullptr };

                                // node - 1, loop around if node == 0
                                submitDesc.nodeMask = 1 << ((node + adapter->queryNodeCount() -1) % adapter->queryNodeCount());
                                CHECK_EQ(queue->submit(submitDesc), llri::result::ErrorIncompatibleNodeMask);
                            }
                        }

                        SUBCASE("[Incorrect usage] CommandList not ready")
                        {
                            llri::submit_desc submitDesc{ nodeMask, 1, nullptr, 0, nullptr, 0, nullptr, nullptr };

                            submitDesc.commandLists = &emptyCmdList;
                            CHECK_EQ(queue->submit(submitDesc), llri::result::ErrorInvalidState);

                            submitDesc.commandLists = &recordingCmdList;
                            CHECK_EQ(queue->submit(submitDesc), llri::result::ErrorInvalidState);
                        }

                        SUBCASE("[Incorrect usage] desc.numCommandLists == 0")
                        {
                            llri::submit_desc submitDesc{ nodeMask, 0, &readyCmdList, 0, nullptr, 0, nullptr, nullptr };
                            CHECK_EQ(queue->submit(submitDesc), llri::result::ErrorInvalidUsage);
                        }

                        SUBCASE("[Incorrect usage] desc.commandLists == nullptr")
                        {
                            llri::submit_desc submitDesc{ nodeMask, 1, nullptr, 0, nullptr, 0, nullptr, nullptr };
                            CHECK_EQ(queue->submit(submitDesc), llri::result::ErrorInvalidUsage);
                        }

                        SUBCASE("[Incorrect usage] a pointer in desc.commandLists == nullptr")
                        {
                            std::array<llri::CommandList*, 2> cmdLists {
                                readyCmdList,
                                nullptr
                            };

                            llri::submit_desc submitDesc{ nodeMask, cmdLists.size(), cmdLists.data(), 0, nullptr, 0, nullptr, nullptr };
                            CHECK_EQ(queue->submit(submitDesc), llri::result::ErrorInvalidUsage);
                        }

                        SUBCASE("[Incorrect usage] desc.numWaitSemaphores > 0 and desc.waitSemaphores == nullptr")
                        {
                            llri::submit_desc submitDesc{ nodeMask, 1, &readyCmdList, 1, nullptr, 0, nullptr, nullptr };
                            CHECK_EQ(queue->submit(submitDesc), llri::result::ErrorInvalidUsage);
                        }

                        SUBCASE("[Incorrect usage] a pointer in desc.waitSemaphores == nullptr")
                        {
                            std::array<llri::Semaphore*, 1> semaphores {
                                nullptr
                            };
                            llri::submit_desc submitDesc{ nodeMask, 1, &readyCmdList, semaphores.size(), semaphores.data(), 0, nullptr, nullptr };
                            CHECK_EQ(queue->submit(submitDesc), llri::result::ErrorInvalidUsage);
                        }

                        SUBCASE("[Incorrect usage] desc.numSignalSemaphores > 0 and desc.signalSemaphores == nullptr")
                        {
                            llri::submit_desc submitDesc{ nodeMask, 1, &readyCmdList, 0, nullptr, 1, nullptr, nullptr };
                            CHECK_EQ(queue->submit(submitDesc), llri::result::ErrorInvalidUsage);
                        }

                        SUBCASE("[Incorrect usage] a pointer in desc.signalSemaphores == nullptr")
                        {
                            std::array<llri::Semaphore*, 1> semaphores{
                                nullptr
                            };
                            llri::submit_desc submitDesc{ nodeMask, 1, &readyCmdList, 0, nullptr, semaphores.size(), semaphores.data(), nullptr };
                            CHECK_EQ(queue->submit(submitDesc), llri::result::ErrorInvalidUsage);
                        }

                        SUBCASE("[Incorrect usage] fence was already signaled")
                        {
                            llri::submit_desc submitDesc{ nodeMask, 1, &readyCmdList, 0, nullptr, 0, nullptr, signaledFence };
                            CHECK_EQ(queue->submit(submitDesc), llri::result::ErrorAlreadySignaled);
                        }
                    }
                }

                device->destroyFence(defaultFence);
                device->destroyFence(signaledFence);
                device->destroyCommandGroup(group);
            }
        }
    }

    instance->destroyDevice(device);
    llri::destroyInstance(instance);
}
