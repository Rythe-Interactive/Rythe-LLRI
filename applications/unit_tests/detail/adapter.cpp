#include <llri/llri.hpp>
namespace llri = legion::graphics::llri;

#include <doctest/doctest.h>

TEST_CASE("Adapter")
{
    llri::Instance* instance = nullptr;
    llri::instance_desc desc{ 0, nullptr, "", {} };
    REQUIRE_EQ(llri::createInstance(desc, &instance), llri::result::Success);

    std::vector<llri::Adapter*> adapters;
    REQUIRE_EQ(instance->enumerateAdapters(&adapters), llri::result::Success);

    for (llri::Adapter* adapter : adapters)
    {
        SUBCASE("Adapter::queryInfo()")
        {
            SUBCASE("[Incorrect usage] info == nullptr")
            {
                CHECK_EQ(adapter->queryInfo(nullptr), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Correct usage] info != nullptr")
            {
                llri::adapter_info info;
                CHECK_EQ(adapter->queryInfo(&info), llri::result::Success);
            }

            SUBCASE("[Correct usage] adapter_info data")
            {
                llri::adapter_info info;
                CHECK_EQ(adapter->queryInfo(&info), llri::result::Success);

                CHECK(info.adapterType <= llri::adapter_type::MaxEnum);
                CHECK_NE(info.vendorId, 0);
                CHECK_NE(info.adapterId, 0);
                CHECK_NE(info.adapterName, "");
            }
        }

        SUBCASE("Adapter::queryFeatures()")
        {
            SUBCASE("[Incorrect usage] features == nullptr")
            {
                CHECK_EQ(adapter->queryFeatures(nullptr), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Correct usage] features != nullptr")
            {
                llri::adapter_features features;
                auto r = adapter->queryFeatures(&features);
                CHECK_UNARY(r == llri::result::Success || r == llri::result::ErrorIncompatibleDriver || r == llri::result::ErrorDeviceLost);
            }

            SUBCASE("[Correct usage] adapter_features data")
            {
                //Reserved for future use
            }
        }

        SUBCASE("Adapter::queryExtensionSupport()")
        {
            SUBCASE("[Incorrect usage] supported == nullptr")
            {
                CHECK_EQ(adapter->queryExtensionSupport(llri::adapter_extension_type::MaxEnum, nullptr), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Correct usage] invalid extension type")
            {
                bool supported = false;
                CHECK_EQ(adapter->queryExtensionSupport(static_cast<llri::adapter_extension_type>(UINT_MAX), &supported), llri::result::Success);
            }

            SUBCASE("[Correct usage] supported != nullptr and extension type is valid")
            {
                //Reserved for future use
            }
        }
    }

    llri::destroyInstance(instance);
}
