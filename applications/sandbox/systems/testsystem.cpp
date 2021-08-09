/**
 * Sandbox is a testing area for LLRI development.
 * The code written in sandbox should be up to spec but may not contain the best practices or cleanest examples.
 *
 * See the samples for recommended usage and more detailed comments.
 */

#include "testsystem.hpp"

//#define LLRI_DISABLE_VALIDATION //uncommenting this disables API validation (see docs)
//#define LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING //uncommenting this disables implementation message polling
#include <llri/llri.hpp>

void callback(const llri::validation_callback_severity& severity, const llri::validation_callback_source& source, const char* message, void* userData)
{
    lgn::log::severity sev = lgn::log::severity_info;
    switch (severity)
    {
        case llri::validation_callback_severity::Verbose:
            sev = lgn::log::severity_trace;
            break;
        case llri::validation_callback_severity::Info:
            //Even though this semantically maps to info, we'd recommend running this on the trace severity to avoid the excessive info logs that some APIs output
            sev = lgn::log::severity_trace;
            break;
        case llri::validation_callback_severity::Warning:
            sev = lgn::log::severity_warn;
            break;
        case llri::validation_callback_severity::Error:
            sev = lgn::log::severity_error;
            break;
        case llri::validation_callback_severity::Corruption:
            sev = lgn::log::severity_error;
    }

    lgn::log::println(sev, "LLRI [{}]: {}", to_string(source), message);
}

void TestSystem::setup()
{
    using namespace legion;
    log::filter(log::severity_debug);

    //Select Instance Extensions
    std::vector<llri::instance_extension> instanceExtensions;
    if (llri::queryInstanceExtensionSupport(llri::instance_extension_type::APIValidation))
        instanceExtensions.emplace_back(llri::instance_extension_type::APIValidation, llri::api_validation_ext { true });
    if (llri::queryInstanceExtensionSupport(llri::instance_extension_type::GPUValidation))
        instanceExtensions.emplace_back(llri::instance_extension_type::GPUValidation, llri::gpu_validation_ext { true });

    const llri::instance_desc instanceDesc{
        instanceExtensions.size(), instanceExtensions.data(),
        "sandbox",
        llri::validation_callback_desc { &callback, nullptr }
    };

    //Create instance
    llri::Instance* instance = nullptr;
    llri::result result = createInstance(instanceDesc, &instance);
    if (result != llri::result::Success)
        log::warn("Failed to create LLRI instance: {}", to_string(result));

    //Iterate over adapters
    std::vector<llri::Adapter*> adapters;
    result = instance->enumerateAdapters(&adapters);
    for (llri::Adapter* adapter : adapters)
    {
        //Log adapter info
        llri::adapter_info info;
        result = adapter->queryInfo(&info);
        if (result != llri::result::Success)
            log::warn("Couldn't query adapter info because {}", to_string(result));

        log::info("Found adapter {}", info.adapterName);
        log::info("\tVendor ID: {}", info.vendorId);
        log::info("\tAdapter ID: {}", info.adapterId);
        log::info("\tAdapter Type: {}", to_string(info.adapterType));

        //Query adapter features so we can read the available features to pick the ones we want to actually enable
        llri::adapter_features selectedFeatures {};
        llri::adapter_features availableFeatures{};
        result = adapter->queryFeatures(&availableFeatures);
        if (result != llri::result::Success)
            log::warn("Couldn't query adapter features because {}", to_string(result));

        //Pick extensions
        std::vector<llri::adapter_extension> adapterExtensions;

        //Create device
        llri::device_desc deviceDesc{ adapter, selectedFeatures, adapterExtensions.size(), adapterExtensions.data() };
        llri::Device* device = nullptr;
        result = instance->createDevice(deviceDesc, &device);
        if (result != llri::result::Success)
            log::warn("Couldn't create device because {}", to_string(result));

        instance->destroyDevice(device);
    }

    llri::destroyInstance(instance);
}

void TestSystem::update(lgn::time::span deltaTime)
{

}
