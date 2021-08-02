#include <llri/llri.hpp>
namespace llri = legion::graphics::llri;

#include <doctest/doctest.h>

TEST_SUITE("Device")
{
    TEST_CASE("Instance::createDevice()")
    {
        llri::Instance* instance;
        const llri::instance_desc desc{ 0, nullptr, "", {} };
        REQUIRE_EQ(llri::createInstance(desc, &instance), llri::result::Success);

        std::vector<llri::Adapter*> adapters;
        REQUIRE_EQ(instance->enumerateAdapters(&adapters), llri::result::Success);

        SUBCASE("[Incorrect usage] device == nullptr")
        {
            llri::device_desc ddesc{};
            CHECK_EQ(instance->createDevice(ddesc, nullptr), llri::result::ErrorInvalidUsage);
        }

        SUBCASE("[Incorrect usage] adapter == nullptr")
        {
            llri::Device* device = nullptr;
            llri::device_desc ddesc{};
            CHECK_EQ(instance->createDevice(ddesc, &device), llri::result::ErrorInvalidUsage);
        }

        for (auto* adapter : adapters)
        {
            llri::Device* device = nullptr;
            llri::device_desc ddesc{ adapter, llri::adapter_features{}, 0, nullptr };

            SUBCASE("[Incorrect usage] numExtensions > 0 && extensions == nullptr")
            {
                ddesc.numExtensions = 1;
                ddesc.extensions = nullptr;
                CHECK_EQ(instance->createDevice(ddesc, &device), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Correct usage] numExtensions == 0 && extensions == nullptr")
            {
                ddesc.numExtensions = 0;
                ddesc.extensions = nullptr;
                auto r = instance->createDevice(ddesc, &device);
                CHECK_UNARY(r == llri::result::Success || r == llri::result::ErrorDeviceLost);
            }

            SUBCASE("[Correct usage] numExtensions > 0 && extensions != nullptr")
            {
                //Reserved for future use (no current extensions supported to test this)
            }

            SUBCASE("[Incorrect usage] invalid extension type")
            {
                //Check reserved for future use. Since there are no adapter extensions, createDevice is currently not expected to iterate over extensions and check for their support, so this remains commented out for now.
                
                //llri::adapter_extension extension{};
                //extension.type = static_cast<llri::adapter_extension_type>(UINT_MAX);

                //ddesc.numExtensions = 1;
                //ddesc.extensions = &extension;
                //CHECK_EQ(instance->createDevice(ddesc, &device), llri::result::ErrorExtensionNotSupported);
            }

            INFO("Extension specific tests are done in \"Device Extensions\"");

            instance->destroyDevice(device);
        }

        llri::destroyInstance(instance);
    }

    TEST_CASE("Instance::destroyDevice()")
    {
        llri::Instance* instance;
        const llri::instance_desc desc{ 0, nullptr, "", {} };
        REQUIRE_EQ(llri::createInstance(desc, &instance), llri::result::Success);

        std::vector<llri::Adapter*> adapters;
        REQUIRE_EQ(instance->enumerateAdapters(&adapters), llri::result::Success);

        for (auto* adapter : adapters)
        {
            llri::Device* device = nullptr;
            llri::device_desc ddesc{ adapter, llri::adapter_features{}, 0, nullptr };
            REQUIRE_EQ(instance->createDevice(ddesc, &device), llri::result::Success);

            SUBCASE("[Correct usage] device != nullptr")
            {
                CHECK_NOTHROW(instance->destroyDevice(device));
            }

            SUBCASE("[Correct usage] device == nullptr")
            {
                CHECK_NOTHROW(instance->destroyDevice(nullptr));
            }
        }
    }
}
