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
            REQUIRE_EQ(instance->createDevice(ddesc, nullptr), llri::result::ErrorInvalidUsage);
        }

        SUBCASE("[Incorrect usage] adapter == nullptr")
        {
            llri::Device* device = nullptr;
            llri::device_desc ddesc{};
            REQUIRE_EQ(instance->createDevice(ddesc, &device), llri::result::ErrorInvalidUsage);
        }

        for (auto* adapter : adapters)
        {
            SUBCASE("[Incorrect usage] numExtensions > 0 && extensions == nullptr")
            {

            }

            SUBCASE("[Correct usage] numExtensions == 0 && extensions == nullptr")
            {

            }

            SUBCASE("[Correct usage] numExtensions > 0 && extensions != nullptr")
            {

            }

            SUBCASE("[Incorrect usage] invalid extension type")
            {

            }

            INFO("Extension specific tests are done in \"Device Extensions\"");

            llri::device_desc ddesc{
                adapter,
                llri::adapter_features{},
                0, nullptr
            };
        }

        llri::destroyInstance(instance);
    }

    TEST_CASE("Instance::destroyDevice()")
    {
        
    }
}
