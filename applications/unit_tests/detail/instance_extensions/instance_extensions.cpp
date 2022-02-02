/**
 * @file instance_extensions.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <string>
#include <doctest/doctest.h>

#include <detail/instance_extensions/surface_win32.hpp>
#include <detail/instance_extensions/surface_cocoa.hpp>
#include <detail/instance_extensions/surface_xlib.hpp>
#include <detail/instance_extensions/surface_xcb.hpp>

TEST_SUITE("Instance Extensions")
{
    TEST_CASE("queryInstanceExtensionSupport()")
    {
        SUBCASE("[Correct usage] invalid extension type")
        {
            CHECK_EQ(llri::queryInstanceExtensionSupport(static_cast<llri::instance_extension>(UINT_MAX)), false);
        }
    }

    TEST_CASE("Instance creation with extension [ext] enabled")
    {
        llri::Instance* instance = nullptr;
        llri::instance_desc desc{ };
        
        for (size_t ext = 0; ext <= static_cast<size_t>(llri::instance_extension::MaxEnum); ext++)
        {
            const auto extension = static_cast<llri::instance_extension>(ext);
            const auto name = llri::to_string(extension);
            
            SUBCASE(name.c_str())
            {
                auto ext = llri::instance_extension::DriverValidation;
                desc.numExtensions = 1;
                desc.extensions = &ext;

                // By checking for support first, we can determine the required llri::createInstance result
                const bool supported = llri::queryInstanceExtensionSupport(ext);
                std::string msg = name + std::string(" is ") + (supported ? std::string("supported") : std::string("not supported"));
                INFO(msg.data());

                if (supported)
                    CHECK_EQ(llri::createInstance(desc, &instance), llri::result::Success);
                else
                    CHECK_EQ(llri::createInstance(desc, &instance), llri::result::ErrorExtensionNotSupported);
                
                llri::destroyInstance(instance);
            }
        }
    }
    
    TEST_CASE("Using instance extensions")
    {
        SUBCASE("instance_extension::SurfaceWin32")
            testInstanceSurfaceWin32();
        
        SUBCASE("instance_extension::SurfaceCocoa")
            testInstanceSurfaceCocoa();

        SUBCASE("instance_extension::SurfaceXlib")
            testInstanceSurfaceXlib();

        SUBCASE("instance_extension::SurfaceXcb")
            testInstanceSurfaceXcb();
    }
}
