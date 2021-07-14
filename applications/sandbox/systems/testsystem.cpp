#include "testsystem.hpp"
#include <llri/llri.hpp>

void TestSystem::setup()
{
    using namespace legion::graphics;
    using namespace legion;
    log::filter(log::severity_debug);

    //Select Instance Extensions
    std::vector<llri::InstanceExtension> instanceExtensions;
    if (llri::queryInstanceExtensionSupport(llri::InstanceExtensionType::APIValidation))
        instanceExtensions.emplace_back(llri::InstanceExtensionType::APIValidation, llri::APIValidationEXT { true });
    if (llri::queryInstanceExtensionSupport(llri::InstanceExtensionType::GPUValidation))
        instanceExtensions.emplace_back(llri::InstanceExtensionType::GPUValidation, llri::GPUValidationEXT { true });

    const llri::InstanceDesc instanceDesc{ instanceExtensions.size(), instanceExtensions.data(), "sandbox" };

    //Create instance
    llri::Instance instance = nullptr;
    llri::Result result = createInstance(instanceDesc, &instance);
    if (result != llri::Result::Success)
        log::warn("Failed to create LLRI instance: {}", to_string(result));

    //Iterate over adapters
    std::vector<llri::Adapter> adapters;
    result = instance->enumerateAdapters(&adapters);
    for (llri::Adapter adapter : adapters)
    {
        //Log adapter info
        llri::AdapterInfo info;
        result = adapter->queryInfo(&info);
        if (result != llri::Result::Success)
            log::warn("Couldn't query adapter info because {}", to_string(result));

        log::info("Found adapter {}", info.adapterName);
        log::info("\tVendor ID: {}", info.vendorId);
        log::info("\tAdapter ID: {}", info.adapterId);
        log::info("\tAdapter Type: {}", to_string(info.adapterType));

        //Query adapter features so we can read the available features to pick the ones we want to actually enable
        llri::AdapterFeatures selectedFeatures {};
        llri::AdapterFeatures availableFeatures{};
        result = adapter->queryFeatures(&availableFeatures);
        if (result != llri::Result::Success)
            log::warn("Couldn't query adapter features because {}", to_string(result));

        //Pick extensions
        std::vector<llri::AdapterExtension> adapterExtensions;

        //Create device
        llri::DeviceDesc deviceDesc{ adapter, selectedFeatures, adapterExtensions.size(), adapterExtensions.data() };
        llri::Device device = nullptr;
        result = instance->createDevice(deviceDesc, &device);
        if (result != llri::Result::Success)
            log::warn("Couldn't create device because {}", to_string(result));

        instance->destroyDevice(device);
    }

    llri::destroyInstance(instance);
}

void TestSystem::update(lgn::time::span deltaTime)
{

}
