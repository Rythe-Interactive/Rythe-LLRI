/**
 * @file adapter.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <doctest/doctest.h>
#include <helpers.hpp>

TEST_CASE("Adapter")
{
    llri::Instance* instance = helpers::defaultInstance();

    std::vector<llri::Adapter*> adapters;
    REQUIRE_EQ(instance->enumerateAdapters(&adapters), llri::result::Success);

    for (llri::Adapter* adapter : adapters)
    {
        SUBCASE("Adapter::queryInfo()")
        {
            llri::adapter_info info = adapter->queryInfo();
            CHECK(info.adapterType <= llri::adapter_type::MaxEnum);
            CHECK_NE(info.vendorId, 0);
            CHECK_NE(info.adapterName, "");
        }

        SUBCASE("Adapter::queryFeatures()")
        {
            llri::adapter_features features = adapter->queryFeatures();

            // reserved for future use
            (void)features;
        }

        SUBCASE("Adapter::queryExtensionSupport()")
        {
            SUBCASE("[Correct usage] invalid extension type")
            {
                CHECK_EQ(adapter->queryExtensionSupport(static_cast<llri::adapter_extension>(UINT_MAX)), false);
            }

            SUBCASE("[Correct usage] supported != nullptr and extension type is valid")
            {
                // Reserved for future use
            }
        }

        SUBCASE("Adapter::queryQueueCount()")
        {
            SUBCASE("[Incorrect usage] invalid queue_type value")
            {
                CHECK_EQ(adapter->queryQueueCount(static_cast<llri::queue_type>(UINT_MAX)), 0);
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
    }

    llri::destroyInstance(instance);
}
