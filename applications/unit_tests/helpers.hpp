/**
 * @file helpers.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */
#pragma once

#include <doctest/doctest.h>
#include <llri/llri.hpp>

namespace helpers
{
    inline llri::Instance* defaultInstance()
    {
        llri::Instance* instance;
        const llri::instance_desc desc{};
        REQUIRE_EQ(llri::createInstance(desc, &instance), llri::result::Success);
        return instance;
    }

    inline llri::Adapter* selectAdapter(llri::Instance* instance)
    {
        std::vector<llri::Adapter*> adapters;
        REQUIRE_EQ(instance->enumerateAdapters(&adapters), llri::result::Success);
        return adapters[0];
    }

    inline llri::Device* defaultDevice(llri::Instance* instance, llri::Adapter* adapter)
    {
        llri::Device* device = nullptr;
        llri::queue_desc queueDesc { llri::queue_type::Graphics, llri::queue_priority::Normal };
        const llri::device_desc ddesc{ adapter, llri::adapter_features{}, 0, nullptr, 1, &queueDesc};
        REQUIRE_EQ(instance->createDevice(ddesc, &device), llri::result::Success);
        return device;
    }
}
