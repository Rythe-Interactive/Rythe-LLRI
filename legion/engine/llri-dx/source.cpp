#include <llri/llri.hpp>
#include <graphics/directx/d3d12.h>
#include <dxgi1_6.h>

namespace legion::graphics::llri
{
    namespace internal
    {
        Result createAPIValidationEXT(const APIValidationEXT& ext, void** output);
        Result createGPUValidationEXT(const GPUValidationEXT& ext, void** output);
        Result mapHRESULT(const HRESULT& value);
    }

    Result createInstance(const InstanceDesc& desc, Instance* instance)
    {
        if (instance == nullptr)
            return Result::ErrorInvalidUsage;
        if (desc.numExtensions > 0 && desc.extensions == nullptr)
            return Result::ErrorInvalidUsage;

        auto* result = new InstanceT();
        UINT factoryFlags = 0;

        for (uint32_t i = 0; i < desc.numExtensions; i++)
        {
            auto& extension = desc.extensions[i];
            Result extensionCreateResult;

            if (extension.data == nullptr)
            {
                destroyInstance(result);
                return Result::ErrorInvalidUsage;
            }

            switch (extension.type)
            {
                case InstanceExtensionType::APIValidation:
                {
                    const APIValidationEXT ext = *(APIValidationEXT*)extension.data;
                    extensionCreateResult = internal::createAPIValidationEXT(ext, &result->m_debugAPI);
                    if (extensionCreateResult == Result::Success)
                        factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
                    break;
                }
                case InstanceExtensionType::GPUValidation:
                {
                    const GPUValidationEXT ext = *(GPUValidationEXT*)extension.data;
                    extensionCreateResult = internal::createGPUValidationEXT(ext, &result->m_debugGPU);
                    break;
                }
                default:
                {
                    destroyInstance(result);
                    return Result::ErrorExtensionNotSupported;
                }
            }

            if (extensionCreateResult != Result::Success)
            {
                destroyInstance(result);
                return extensionCreateResult;
            }
        }

        //Attempt to create factory
        IDXGIFactory* factory = nullptr;
        const HRESULT factoryCreateResult = CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory));
        if (FAILED(factoryCreateResult))
        {
            destroyInstance(result);
            return internal::mapHRESULT(factoryCreateResult);
        }

        //Store factory and return result
        result->m_ptr = factory;
        *instance = result;
        return Result::Success;
    }

    void destroyInstance(Instance instance)
    {
        if (!instance)
            return;

        if (instance->m_debugAPI)
            static_cast<ID3D12Debug*>(instance->m_debugAPI)->Release();

        if (instance->m_debugGPU)
            static_cast<ID3D12Debug1*>(instance->m_debugGPU)->Release();

        delete instance;
    }
}
