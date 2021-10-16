/**
 * @file source.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <iostream>

void callback(llri::message_severity severity, llri::message_source source, const char* message, void* userData)
{
    std::cout << "LLRI " << to_string(source) << " " << to_string(severity) << ": " << message << "\n";
}

int main()
{
    llri::setMessageCallback(&callback);

    // This example expands on the 001_validation example by adding driver and gpu validation through Instance extensions. Both of these validation features are handled by the implementation, so they may or may not be available depending on system configuration.
    // Whenever implementation validation is enabled, it will forward messages to the callback set in llri::setMessageCallback(), so make sure to set up a callback to receive these messages.
    std::vector<llri::instance_extension> extensions;

    // We can query for an extension's support using llri::queryInstanceExtensionSupport()
    // if an extension isn't supported but it is added to the extension list anyways, createInstance() will return result::ErrorExtensionNotSupported.
    if (llri::queryInstanceExtensionSupport(llri::instance_extension_type::DriverValidation))
    {
        llri::instance_extension extension{};
        extension.type = llri::instance_extension_type::DriverValidation;
        extension.driverValidation = llri::driver_validation_ext { true };
        extensions.push_back(extension);
    }

    // The much shorter constructor for instance_extension may also be used.
    if (llri::queryInstanceExtensionSupport(llri::instance_extension_type::GPUValidation))
        extensions.emplace_back(llri::instance_extension_type::GPUValidation, llri::gpu_validation_ext { true });

    const llri::instance_desc instanceDesc = { static_cast<uint32_t>(extensions.size()), extensions.data(), // We can pass the extensions through the instance_desc
        "validation_ext",
    };

    llri::Instance* instance;
    const llri::result r = llri::createInstance(instanceDesc, &instance);
    std::cout << "Instance create result: " << to_string(r) << "\n";
    return 0;
}
