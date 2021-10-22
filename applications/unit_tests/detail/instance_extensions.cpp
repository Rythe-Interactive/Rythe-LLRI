/**
 * @file instance_extensions.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <string>
#include <doctest/doctest.h>

TEST_SUITE("Instance Extensions")
{
    TEST_CASE("queryInstanceExtensionSupport()")
    {
        SUBCASE("[Correct usage] invalid extension type")
        {
            CHECK_EQ(llri::queryInstanceExtensionSupport(static_cast<llri::instance_extension>(UINT_MAX)), false);
        }
    }

    TEST_CASE("extensions")
    {
        llri::Instance* instance = nullptr;
        llri::instance_desc desc{ };

        SUBCASE("instance_extension::DriverValidation")
        {
            auto ext = llri::instance_extension::DriverValidation;
            desc.numExtensions = 1;
            desc.extensions = &ext;

            // By checking for support first, we can determine the expected llri::createInstance result
            const bool supported = llri::queryInstanceExtensionSupport(llri::instance_extension::DriverValidation);
            std::string msg = std::string("instance_extension::DriverValidation is ") + (supported ? std::string("supported") : std::string("not supported"));
            INFO(msg.data());

            if (supported)
                CHECK_EQ(llri::createInstance(desc, &instance), llri::result::Success);
            else
                CHECK_EQ(llri::createInstance(desc, &instance), llri::result::ErrorExtensionNotSupported);
        }
        SUBCASE("instance_extension::GPUValidation")
        {
            auto ext = llri::instance_extension::GPUValidation;
            desc.numExtensions = 1;
            desc.extensions = &ext;

            // By checking for support first, we can determine the expected llri::createInstance result
            const bool supported = llri::queryInstanceExtensionSupport(llri::instance_extension::GPUValidation);
            std::string msg = std::string("instance_extension::GPUValidation is ") + (supported ? std::string("supported") : std::string("not supported"));
            INFO(msg.data());

            if (supported)
                CHECK_EQ(llri::createInstance(desc, &instance), llri::result::Success);
            else
                CHECK_EQ(llri::createInstance(desc, &instance), llri::result::ErrorExtensionNotSupported);
        }

        llri::destroyInstance(instance);
    }
}
