#include <llri/llri.hpp>
namespace llri = legion::graphics::llri;

#include <doctest/doctest.h>

TEST_SUITE("Instance")
{
    TEST_CASE("createInstance()")
    {
        CHECK(llri::createInstance({}, nullptr) == llri::result::ErrorInvalidUsage);

        llri::Instance* instance = nullptr;
        llri::instance_desc desc{ };

        SUBCASE("[Incorrect usage] numExtensions > 0 && extensions == nullptr")
        {
            desc.numExtensions = 1;
            CHECK_EQ(llri::createInstance(desc, &instance), llri::result::ErrorInvalidUsage);
        }

        SUBCASE("[Correct usage] numExtensions == 0 && extensions == nullptr")
        {
            desc.numExtensions = 0;
            CHECK_EQ(llri::createInstance(desc, &instance), llri::result::Success);
        }

        SUBCASE("[Correct usage] numExtensions > 0 && extensions != nullptr")
        {
            llri::instance_extension extension{ llri::instance_extension_type::APIValidation, llri::api_validation_ext { false } };
            desc.numExtensions = 1;
            desc.extensions = &extension;

            auto result = llri::createInstance(desc, &instance);
            CHECK_UNARY(result == llri::result::Success || result == llri::result::ErrorExtensionNotSupported);
        }
        
        SUBCASE("[Incorrect usage] invalid extension type")
        {
            llri::instance_extension extension{ (llri::instance_extension_type)UINT_MAX, llri::api_validation_ext { false } };

            desc.numExtensions = 1;
            desc.extensions = &extension;

            CHECK_EQ(llri::createInstance(desc, &instance), llri::result::ErrorExtensionNotSupported);
        }

        INFO("Extension specific tests are done in \"Instance Extensions\"");

        SUBCASE("[Correct usage] applicationName == nullptr")
        {
            desc.applicationName = nullptr;
            CHECK_EQ(llri::createInstance(desc, &instance), llri::result::Success);
        }

        SUBCASE("[Correct usage] callbackDesc.callback == nullptr")
        {
            desc.callbackDesc.callback = nullptr;
            CHECK_EQ(llri::createInstance(desc, &instance), llri::result::Success);
        }

        llri::destroyInstance(instance);
    }

    TEST_CASE("destroyInstance()")
    {
        SUBCASE("[Correct usage] instance == nullptr")
        {
            CHECK_NOTHROW(llri::destroyInstance(nullptr));
        }

        SUBCASE("[Correct usage] instance != nullptr")
        {
            llri::Instance* instance;
            const llri::instance_desc desc{ 0, nullptr, "", {} };
            REQUIRE_EQ(llri::createInstance(desc, &instance), llri::result::Success);

            CHECK_NOTHROW(llri::destroyInstance(instance));
        }
    }

    TEST_CASE("Instance::enumerateAdapters")
    {
        llri::Instance* instance;
        const llri::instance_desc desc{ 0, nullptr, "", {} };
        REQUIRE_EQ(llri::createInstance(desc, &instance), llri::result::Success);

        SUBCASE("[Incorrect usage] adapters == nullptr")
        {
            CHECK_EQ(instance->enumerateAdapters(nullptr), llri::result::ErrorInvalidUsage);
        }

        SUBCASE("[Correct usage] adapters != nullptr")
        {
            std::vector<llri::Adapter*> adapters;
            const llri::result result = instance->enumerateAdapters(&adapters);
            //Any of the following is a valid result value for enumerateAdapters
            CHECK_UNARY(
                result == llri::result::Success ||
                result == llri::result::ErrorOutOfHostMemory ||
                result == llri::result::ErrorOutOfDeviceMemory ||
                result == llri::result::ErrorInitializationFailed
            );
        }

        llri::destroyInstance(instance);
    }
}
