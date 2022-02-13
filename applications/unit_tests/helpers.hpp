/**
 * @file helpers.hpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <doctest/doctest.h>
#include <llri/llri.hpp>
#include <sstream>

namespace detail
{
    inline std::vector<std::string> split (const std::string &s, char delim)
    {
        std::vector<std::string> result;
        std::stringstream ss (s);
        std::string item;

        while (getline (ss, item, delim)) {
            result.push_back (item);
        }

        return result;
    }

    /**
     * @brief Utility function for hashing strings  in compile time
    */
    template<size_t N>
    constexpr uint64_t nameHash(const char(&name)[N]) noexcept
    {
        uint64_t hash = 0xcbf29ce484222325;
        constexpr uint64_t prime = 0x00000100000001b3;

        for (size_t i = 0; i < N; i++)
        {
            if (name[i] == '\0')
                break;
            
            hash = hash ^ static_cast<const uint8_t>(name[i]);
            hash *= prime;
        }

        return hash;
    }

    inline llri::Instance* defaultInstance()
    {
        llri::Instance* instance;

        std::vector<llri::instance_extension> extensions;
        if (llri::queryInstanceExtensionSupport(llri::instance_extension::DriverValidation))
            extensions.push_back(llri::instance_extension::DriverValidation);

        const llri::instance_desc desc{ static_cast<uint32_t>(extensions.size()), extensions.data(), "unit test instance"};
        REQUIRE_EQ(llri::createInstance(desc, &instance), llri::result::Success);
        return instance;
    }

    inline llri::Instance* createInstanceWithExtension(llri::instance_extension ext)
    {
        if (llri::queryInstanceExtensionSupport(ext) == false)
            return nullptr;
        
        llri::Instance* instance;

        std::vector<llri::instance_extension> extensions;
        extensions.push_back(ext);
        
        if (llri::queryInstanceExtensionSupport(llri::instance_extension::DriverValidation))
            extensions.push_back(llri::instance_extension::DriverValidation);

        const llri::instance_desc desc{ static_cast<uint32_t>(extensions.size()), extensions.data(), "unit test instance"};
        REQUIRE_EQ(llri::createInstance(desc, &instance), llri::result::Success);
        return instance;
    }

    template<typename Func, typename ...Args>
    inline void iterateAdapters(llri::Instance* instance, Func&& function, Args&&... args)
    {
        std::vector<llri::Adapter*> adapters;
        REQUIRE_EQ(instance->enumerateAdapters(&adapters), llri::result::Success);
        REQUIRE_UNARY(adapters.size() > 0);
        
        constexpr size_t adapterStr = nameHash(LLRI_SELECTED_TEST_ADAPTERS);
        constexpr size_t all = nameHash("All");
        
        std::vector<llri::Adapter*> selectedAdapters;
        
        if constexpr (adapterStr == all)
        {
            selectedAdapters = adapters;
        }
        else
        {
            auto selectedAdapterNames = split(LLRI_SELECTED_TEST_ADAPTERS, ';');
            
            for (auto* adapter : adapters)
            {
                llri::adapter_info info = adapter->queryInfo();
                
                if (std::find(selectedAdapterNames.begin(), selectedAdapterNames.end(), info.adapterName) != selectedAdapterNames.end())
                {
                    selectedAdapters.push_back(adapter);
                }
            }
            
            REQUIRE_EQ(selectedAdapters.size(), selectedAdapterNames.size());
        }
        
        for (auto* adapter : selectedAdapters)
        {
            std::invoke(std::forward<Func>(function), adapter, std::forward<Args>(args)...);
        }
    }

    inline llri::Device* defaultDevice(llri::Instance* instance, llri::Adapter* adapter)
    {
        llri::Device* device = nullptr;

        uint8_t graphicsQueueCount = adapter->queryQueueCount(llri::queue_type::Graphics);
        uint8_t computeQueueCount = adapter->queryQueueCount(llri::queue_type::Compute);
        uint8_t transferQueueCount = adapter->queryQueueCount(llri::queue_type::Transfer);

        std::vector<llri::queue_desc> queues;

        if (graphicsQueueCount > 0)
            queues.push_back(llri::queue_desc{ llri::queue_type::Graphics, llri::queue_priority::Normal });
        if (computeQueueCount > 0)
            queues.push_back(llri::queue_desc{ llri::queue_type::Compute, llri::queue_priority::Normal });
        if (transferQueueCount > 0)
            queues.push_back(llri::queue_desc{ llri::queue_type::Transfer, llri::queue_priority::Normal });

        const llri::device_desc ddesc{ adapter, llri::adapter_features{}, 0, nullptr, static_cast<uint32_t>(queues.size()), queues.data() };
        REQUIRE_EQ(instance->createDevice(ddesc, &device), llri::result::Success);
        return device;
    }

    inline llri::queue_type availableQueueType(llri::Adapter* adapter)
    {
        for (size_t type = 0; type <= static_cast<uint8_t>(llri::queue_type::MaxEnum); type++)
        {
            uint8_t count = adapter->queryQueueCount(static_cast<llri::queue_type>(type));

            if (count > 0)
                return static_cast<llri::queue_type>(type);
        }

        FAIL("No available queue for this adapter");
        return llri::queue_type::MaxEnum;
    }

    inline llri::CommandGroup* defaultCommandGroup(llri::Device* device, llri::queue_type type)
    {
        llri::CommandGroup* cmdGroup;
        REQUIRE_EQ(device->createCommandGroup(type, &cmdGroup), llri::result::Success);
        return cmdGroup;
    }

    inline llri::CommandList* defaultCommandList(llri::CommandGroup* group, uint32_t nodeMask, llri::command_list_usage usage)
    {
        llri::command_list_alloc_desc desc{};
        desc.nodeMask = nodeMask;
        desc.usage = usage;

        llri::CommandList* cmd;
        REQUIRE_EQ(group->allocate(desc, &cmd), llri::result::Success);
        return cmd;
    }

    inline llri::Fence* defaultFence(llri::Device* device, bool signaled)
    {
        llri::fence_flags flags = signaled ? llri::fence_flag_bits::Signaled : llri::fence_flag_bits::None;
        llri::Fence* result;
        REQUIRE_EQ(device->createFence(flags, &result), llri::result::Success);
        return result;
    }
}
