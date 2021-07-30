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

        SUBCASE("numExtensions > 0 && extensions == nullptr")
        {
            desc.numExtensions = 1;
            CHECK_EQ(llri::createInstance(desc, &instance), llri::result::ErrorInvalidUsage);
        }

        SUBCASE("numExtensions == 0 && extensions == nullptr")
        {
            desc.numExtensions = 0;
            CHECK_EQ(llri::createInstance(desc, &instance), llri::result::Success);
        }

        SUBCASE("numExtensions > 0 && extensions != nullptr")
        {
            llri::instance_extension extension{ llri::instance_extension_type::APIValidation, llri::api_validation_ext { false } };
            desc.numExtensions = 1;
            desc.extensions = &extension;

            auto result = llri::createInstance(desc, &instance);
            CHECK_UNARY(result == llri::result::Success || result == llri::result::ErrorExtensionNotSupported);
        }

        SUBCASE("Invalid extension type")
        {
            llri::instance_extension extension{ (llri::instance_extension_type)UINT_MAX, llri::api_validation_ext { false } };

            desc.numExtensions = 1;
            desc.extensions = &extension;

            CHECK_EQ(llri::createInstance(desc, &instance), llri::result::ErrorExtensionNotSupported);
        }

        INFO("Extension specific tests are done in \"Instance Extensions\"");

        SUBCASE("applicationName == nullptr")
        {
            desc.applicationName = nullptr;
            CHECK_EQ(llri::createInstance(desc, &instance), llri::result::Success);
        }

        llri::destroyInstance(instance);
    }
}
