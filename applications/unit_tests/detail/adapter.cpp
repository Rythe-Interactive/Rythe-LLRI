/**
 * @file adapter.cpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#include <llri/llri.hpp>
#include <doctest/doctest.h>
#include <helpers.hpp>

TEST_CASE("Adapter")
{
    llri::Instance* instance = detail::defaultInstance();

    detail::iterateAdapters(instance, [](llri::Adapter* adapter) {
        SUBCASE("Adapter::queryInfo()")
        {
            llri::adapter_info info = adapter->queryInfo();
            CHECK(info.adapterType <= llri::adapter_type::MaxEnum);
            CHECK_NE(info.vendorId, 0);
            CHECK_NE(info.adapterName, "");
        }
        
        SUBCASE("Adapter::queryFeatures()")
        {
            [[maybe_unused]] llri::adapter_features features = adapter->queryFeatures();

            // reserved for future use
        }
        
        SUBCASE("Adapter::queryExtensionSupport()")
        {
            SUBCASE("[Correct usage] invalid extension type")
            {
                CHECK_EQ(adapter->queryExtensionSupport(static_cast<llri::adapter_extension>(std::numeric_limits<uint8_t>::max())), false);
            }
        }

        SUBCASE("Adapter::queryQueueCount()")
        {
            SUBCASE("[Incorrect usage] invalid queue_type value")
            {
                CHECK_EQ(adapter->queryQueueCount(static_cast<llri::queue_type>(std::numeric_limits<uint8_t>::max())), 0);
            }
        }

        SUBCASE("Adapter::queryFormatProperties()")
        {
            auto props = adapter->queryFormatProperties();
            CHECK_FALSE(props.empty());
            CHECK_EQ(props.size(), static_cast<size_t>(llri::format::MaxEnum) + 1);

            for (uint8_t f = 0; f <= static_cast<uint8_t>(llri::format::MaxEnum); f++)
            {
                CHECK_NE(props.find(static_cast<llri::format>(f)), props.end());
            }
        }
    });

    llri::destroyInstance(instance);
}
