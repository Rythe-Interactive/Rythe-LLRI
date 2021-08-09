/**
 * Copyright 2021-2021 Leon Brands. All rights served.
 * License: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
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
            CHECK_EQ(llri::queryInstanceExtensionSupport(static_cast<llri::instance_extension_type>(UINT_MAX)), false);
        }
    }

    TEST_CASE("extensions")
    {
        llri::Instance* instance = nullptr;
        llri::instance_desc desc{ };

        SUBCASE("api_validation_ext")
        {
            llri::instance_extension extension{ llri::instance_extension_type::APIValidation, llri::api_validation_ext { true } };
            desc.numExtensions = 1;
            desc.extensions = &extension;

            //By checking for support first, we can determine the expected llri::createInstance result
            const bool supported = llri::queryInstanceExtensionSupport(llri::instance_extension_type::APIValidation);
            std::string msg = std::string("api_validation_ext is ") + (supported ? std::string("supported") : std::string("not supported"));
            INFO(msg.data());

            SUBCASE("[Correct usage] enabling api_validation_ext")
            {
                if (supported)
                    CHECK_EQ(llri::createInstance(desc, &instance), llri::result::Success);
                else
                    CHECK_EQ(llri::createInstance(desc, &instance), llri::result::ErrorExtensionNotSupported);
            }
        }
        SUBCASE("gpu_validation_ext")
        {
            llri::instance_extension extension{ llri::instance_extension_type::GPUValidation, llri::gpu_validation_ext { true } };
            desc.numExtensions = 1;
            desc.extensions = &extension;

            //By checking for support first, we can determine the expected llri::createInstance result
            const bool supported = llri::queryInstanceExtensionSupport(llri::instance_extension_type::GPUValidation);
            std::string msg = std::string("gpu_validation_ext is ") + (supported ? std::string("supported") : std::string("not supported"));
            INFO(msg.data());

            SUBCASE("[Correct usage] enabling gpu_validation_ext")
            {
                if (supported)
                    CHECK_EQ(llri::createInstance(desc, &instance), llri::result::Success);
                else
                    CHECK_EQ(llri::createInstance(desc, &instance), llri::result::ErrorExtensionNotSupported);
            }
        }

        llri::destroyInstance(instance);
    }
}
