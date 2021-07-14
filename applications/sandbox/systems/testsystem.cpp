#include "testsystem.hpp"
#include <llri/llri.hpp>

void TestSystem::setup()
{
    using namespace legion::graphics;
    using namespace legion;
    log::filter(log::severity_debug);

    llri::APIValidationEXT apiValidation{ true };
    llri::GPUValidationEXT gpuValidation{ true };

    std::vector<llri::InstanceExtension> extensions;
    if (llri::queryInstanceExtensionSupport(llri::InstanceExtensionType::APIValidation))
        extensions.emplace_back(llri::InstanceExtensionType::APIValidation, &apiValidation);
    if (llri::queryInstanceExtensionSupport(llri::InstanceExtensionType::GPUValidation))
        extensions.emplace_back(llri::InstanceExtensionType::GPUValidation, &gpuValidation);
    const llri::InstanceDesc desc{ extensions.size(), extensions.data(), "Test" };

    llri::Instance instance = nullptr;
    llri::Result result = createInstance(desc, &instance);

    if (result != llri::Result::Success)
        log::warn("Failed to create LLRI instance: {}", to_string(result));

    llri::destroyInstance(instance);
}

void TestSystem::update(lgn::time::span deltaTime)
{

}
