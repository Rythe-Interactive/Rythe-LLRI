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

            switch (extension.type)
            {
                case InstanceExtensionType::APIValidation:
                {
                    extensionCreateResult = internal::createAPIValidationEXT(extension.apiValidation, &result->m_debugAPI);
                    if (extensionCreateResult == Result::Success)
                        factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
                    break;
                }
                case InstanceExtensionType::GPUValidation:
                {
                    extensionCreateResult = internal::createGPUValidationEXT(extension.gpuValidation, &result->m_debugGPU);
                    break;
                }
                default:
                {
                    extensionCreateResult = Result::ErrorExtensionNotSupported;
                    break;
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

        for (auto& [ptr, adapter] : instance->m_cachedAdapters)
            delete adapter;

        if (instance->m_debugAPI)
            static_cast<ID3D12Debug*>(instance->m_debugAPI)->Release();

        if (instance->m_debugGPU)
            static_cast<ID3D12Debug1*>(instance->m_debugGPU)->Release();

        delete instance;
    }

    Result InstanceT::enumerateAdapters(std::vector<Adapter>* adapters)
    {
        if (adapters == nullptr)
            return Result::ErrorInvalidUsage;

        adapters->clear();

        //Clear internal pointers, lost adapters will have a nullptr internally
        for (auto& [ptr, adapter] : m_cachedAdapters)
            adapter->m_ptr = nullptr;

        IDXGIAdapter* dxgiAdapter = nullptr;
        HRESULT result = 0;
        uint32_t i = 0;
        while (true)
        {
            result = static_cast<IDXGIFactory*>(m_ptr)->EnumAdapters(i, &dxgiAdapter);
            if (result == DXGI_ERROR_NOT_FOUND)
                break;

            //Ignore Microsoft Basic Render Driver
            DXGI_ADAPTER_DESC1 desc;
            static_cast<IDXGIAdapter1*>(dxgiAdapter)->GetDesc1(&desc); //DirectX casts should always be static
            const uint64_t luid = desc.AdapterLuid.LowPart + desc.AdapterLuid.HighPart;
            if (desc.Flags == DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                i++;
                continue;
            }

            if (m_cachedAdapters.find((void*)luid) != m_cachedAdapters.end())
            {
                //Re-assign pointer to found adapters
                m_cachedAdapters[(void*)luid]->m_ptr = dxgiAdapter;
                adapters->push_back(m_cachedAdapters[(void*)luid]);
            }
            else
            {
                Adapter adapter = new AdapterT();
                adapter->m_ptr = dxgiAdapter;

                m_cachedAdapters[(void*)luid] = adapter;
                adapters->push_back(adapter);
            }

            i++;
        }

        return Result::Success;
    }

    Result InstanceT::createDevice(const DeviceDesc& desc, Device* device)
    {
        if (m_ptr == nullptr || device == nullptr || desc.adapter == nullptr)
            return Result::ErrorInvalidUsage;

        if (desc.numExtensions > 0 && desc.extensions == nullptr)
            return Result::ErrorInvalidUsage;

        if (desc.adapter->m_ptr == nullptr)
            return Result::ErrorDeviceLost;

        Device result = new DeviceT();

        D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_0; //12.0 is the bare minimum

        ID3D12Device* dx12Device = nullptr;
        HRESULT r = D3D12CreateDevice(static_cast<IDXGIAdapter*>(desc.adapter->m_ptr), featureLevel, IID_PPV_ARGS(&dx12Device));
        if (FAILED(r))
        {
            destroyDevice(result);
            return internal::mapHRESULT(r);
        }

        result->m_ptr = dx12Device;
        *device = result;
        return Result::Success;
    }

    void InstanceT::destroyDevice(Device device)
    {
        if (!device)
            return;

        if (device->m_ptr)
            static_cast<ID3D12Device*>(device->m_ptr)->Release();

        delete device;
    }

    Result AdapterT::queryInfo(AdapterInfo* info) const
    {
        if (info == nullptr)
            return Result::ErrorInvalidUsage;

        if (m_ptr == nullptr)
            return Result::ErrorDeviceRemoved;

        DXGI_ADAPTER_DESC1 desc;
        static_cast<IDXGIAdapter1*>(m_ptr)->GetDesc1(&desc);

        AdapterInfo result;
        result.vendorId = desc.VendorId;
        result.adapterId = desc.DeviceId;
        const auto description = std::string(desc.Description, desc.Description + 128);
        result.adapterName = description.substr(0, description.find_last_not_of(' '));

        if (desc.Flags == DXGI_ADAPTER_FLAG_REMOTE)
            result.adapterType = AdapterType::Virtual;
        else if (desc.DedicatedVideoMemory > 0)
            result.adapterType = AdapterType::Discrete;
        else // video memory == 0 
            result.adapterType = AdapterType::Integrated;

        *info = result;
        return Result::Success;
    }

    Result AdapterT::queryFeatures(AdapterFeatures* features) const
    {
        if (features == nullptr)
            return Result::ErrorInvalidUsage;

        if (m_ptr == nullptr)
            return Result::ErrorDeviceRemoved;

        HRESULT level12_0 = D3D12CreateDevice(static_cast<IDXGIAdapter*>(m_ptr), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr);
        if (level12_0 == E_FAIL)
            return Result::ErrorIncompatibleDriver;
        else if (FAILED(level12_0)) //no matter what reason, LEVEL_12_0 should always be supported for DX12
            return internal::mapHRESULT(level12_0);

        //Level 1 and 2 can more easily grant us guarantees about feature support
        HRESULT level12_1 = D3D12CreateDevice(static_cast<IDXGIAdapter*>(m_ptr), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr);
        HRESULT level12_2 = D3D12CreateDevice(static_cast<IDXGIAdapter*>(m_ptr), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr);

        AdapterFeatures result;

        //Set all the information in a structured way here

        *features = result;
        return Result::Success;
    }

    bool AdapterT::queryExtensionSupport(const AdapterExtensionType& type) const
    {
        switch (type)
        {
            default:
                break;
        }

        return false;
    }
}
