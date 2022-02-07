/**
 * @file instance_extensions.cpp
 * Copyright (c) 2021 Leon Brands
 * SPDX-License-Identifier: MIT
 */

#include <llri/llri.hpp>
#include <string>
#include <doctest/doctest.h>

#include <detail/instance_extensions/surface_properties.hpp>
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
            CHECK_EQ(llri::queryInstanceExtensionSupport(static_cast<llri::instance_extension>(std::numeric_limits<uint8_t>::max())), false);
        }
    }

    TEST_CASE("Instance creation with extension [ext] enabled")
    {
        llri::Instance* instance = nullptr;
        llri::instance_desc desc{ };
        
        for (size_t e = 0; e <= static_cast<size_t>(llri::instance_extension::MaxEnum); e++)
        {
            auto extension = static_cast<llri::instance_extension>(e);
            const auto name = llri::to_string(extension);
            
            SUBCASE(name.c_str())
            {
                desc.numExtensions = 1;
                desc.extensions = &extension;

                // By checking for support first, we can determine the required llri::createInstance result
                const bool supported = llri::queryInstanceExtensionSupport(extension);
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
        SUBCASE("SurfaceEXT properties")
            testSurfaceProperties();
        
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
