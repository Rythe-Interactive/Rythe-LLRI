/**
 * @file source.cpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#include <llri/llri.hpp>
#include <iostream>

void callback(llri::message_severity severity, llri::message_source source, const char* message, [[maybe_unused]] void* userData)
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
    if (llri::queryInstanceExtensionSupport(llri::instance_extension::DriverValidation))
        extensions.push_back(llri::instance_extension::DriverValidation);

    if (llri::queryInstanceExtensionSupport(llri::instance_extension::GPUValidation))
        extensions.emplace_back(llri::instance_extension::GPUValidation);

    const llri::instance_desc instanceDesc = { static_cast<uint32_t>(extensions.size()), extensions.data(), // We can pass the extensions through the instance_desc
        "validation_extensions",
    };

    llri::Instance* instance;
    const llri::result r = llri::createInstance(instanceDesc, &instance);
    std::cout << "Instance create result: " << to_string(r) << "\n";
    return 0;
}
