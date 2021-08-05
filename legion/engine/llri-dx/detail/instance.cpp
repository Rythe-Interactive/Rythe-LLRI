#include <llri/llri.hpp>
#include <llri-dx/directx.hpp>

namespace legion::graphics::llri
{
    namespace internal
    {
        result createAPIValidationEXT(const api_validation_ext& ext, void** output);
        result createGPUValidationEXT(const gpu_validation_ext& ext, void** output);

        result mapHRESULT(const HRESULT& value);

        void dummyValidationCallback(const validation_callback_severity&, const validation_callback_source&, const char*, void*) { }

        validation_callback_severity mapSeverity(D3D12_MESSAGE_SEVERITY sev)
        {
            switch (sev)
            {
            case D3D12_MESSAGE_SEVERITY_CORRUPTION:
                return validation_callback_severity::Corruption;
            case D3D12_MESSAGE_SEVERITY_ERROR:
                return validation_callback_severity::Error;
            case D3D12_MESSAGE_SEVERITY_WARNING:
                return validation_callback_severity::Warning;
            case D3D12_MESSAGE_SEVERITY_INFO:
                return validation_callback_severity::Info;
            case D3D12_MESSAGE_SEVERITY_MESSAGE:
                return validation_callback_severity::Verbose;
            }

            return validation_callback_severity::Info;
        }
    }

    namespace detail
    {
        result impl_createInstance(const instance_desc& desc, Instance** instance, const bool& enableInternalAPIMessagePolling)
        {
            directx::lazyInitializeDirectX();

            auto* output = new Instance();
            UINT factoryFlags = 0;

            for (uint32_t i = 0; i < desc.numExtensions; i++)
            {
                auto& extension = desc.extensions[i];
                result extensionCreateResult;

                switch (extension.type)
                {
                    case instance_extension_type::APIValidation:
                    {
                        extensionCreateResult = internal::createAPIValidationEXT(extension.apiValidation, &output->m_debugAPI);
                        if (extensionCreateResult == result::Success)
                            factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
                        break;
                    }
                    case instance_extension_type::GPUValidation:
                    {
                        extensionCreateResult = internal::createGPUValidationEXT(extension.gpuValidation, &output->m_debugGPU);
                        break;
                    }
                    default:
                    {
                        if (desc.callbackDesc.callback)
                            desc.callbackDesc(validation_callback_severity::Error, validation_callback_source::Validation, (std::string("createInstance() returned ErrorExtensionNotSupported because the extension type ") + std::to_string((int)extension.type) + " is not recognized.").c_str());

                        extensionCreateResult = result::ErrorExtensionNotSupported;
                        break;
                    }
                }

                if (extensionCreateResult != result::Success)
                {
                    llri::destroyInstance(output);
                    return extensionCreateResult;
                }
            }

            //Store user defined validation callback
            //DirectX creates validation callbacks upon device creation so we just need to store information about this right now.
            output->m_validationCallbackMessenger = nullptr;
            if (enableInternalAPIMessagePolling && desc.callbackDesc.callback)
            {
                output->m_validationCallback = desc.callbackDesc;
                output->m_shouldConstructValidationCallbackMessenger = true;
            }
            else
            {
                output->m_validationCallback = { &internal::dummyValidationCallback, nullptr };
                output->m_shouldConstructValidationCallbackMessenger = false;
            }

            //Attempt to create factory
            IDXGIFactory* factory = nullptr;
            HRESULT factoryCreateResult = directx::CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory));

            //DXGI_CREATE_FACTORY_DEBUG may not be a supported flag if the graphics tools aren't installed
            //so if this the previous call fails, use default factory flags
            if (HRESULT_CODE(factoryCreateResult) == S_FALSE)
                factoryCreateResult = directx::CreateDXGIFactory2(0, IID_PPV_ARGS(&factory));

            //Check for failure
            if (FAILED(factoryCreateResult))
            {
                llri::destroyInstance(output);
                return internal::mapHRESULT(factoryCreateResult);
            }

            //Store factory and return result
            output->m_ptr = factory;
            *instance = output;
            return result::Success;
        }

        void impl_destroyInstance(Instance* instance)
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

        void impl_pollAPIMessages(const validation_callback_desc& validation, messenger_type* messenger)
        {
            if (messenger != nullptr)
            {
                ID3D12InfoQueue* iq = static_cast<ID3D12InfoQueue*>(messenger);
                const auto numMsg = iq->GetNumStoredMessages();

                for (UINT64 i = 0; i < numMsg; ++i)
                {
                    SIZE_T messageLength = 0;
                    iq->GetMessage(i, NULL, &messageLength);

                    D3D12_MESSAGE* pMessage = reinterpret_cast<D3D12_MESSAGE*>(malloc(messageLength));
                    iq->GetMessage(i, pMessage, &messageLength);
                    validation(internal::mapSeverity(pMessage->Severity), validation_callback_source::InternalAPI, pMessage->pDescription);

                    free(pMessage);
                }

                iq->ClearStoredMessages();
            }
        }
    }

    result Instance::impl_enumerateAdapters(std::vector<Adapter*>* adapters)
    {
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
                Adapter* adapter = new Adapter();
                adapter->m_ptr = dxgiAdapter;
                adapter->m_validationCallback = m_validationCallback;
                adapter->m_validationCallbackMessenger = m_validationCallbackMessenger;

                m_cachedAdapters[(void*)luid] = adapter;
                adapters->push_back(adapter);
            }

            i++;
        }

        return result::Success;
    }

    result Instance::impl_createDevice(const device_desc& desc, Device** device) const
    {
        Device* output = new Device();
        output->m_validationCallback = m_validationCallback;

        D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_0; //12.0 is the bare minimum

        ID3D12Device* dx12Device = nullptr;
        HRESULT r = directx::D3D12CreateDevice(static_cast<IDXGIAdapter*>(desc.adapter->m_ptr), featureLevel, IID_PPV_ARGS(&dx12Device));
        if (FAILED(r))
        {
            destroyDevice(output);
            return internal::mapHRESULT(r);
        }
        output->m_ptr = dx12Device;

        if (m_shouldConstructValidationCallbackMessenger)
        {
            ID3D12InfoQueue* iq = nullptr;
            r = dx12Device->QueryInterface(IID_PPV_ARGS(&iq));

            if (SUCCEEDED(r))
                output->m_validationCallbackMessenger = iq;
        }

        *device = output;
        return result::Success;
    }

    void Instance::impl_destroyDevice(Device* device) const
    {
        if (!device)
            return;

        if (device->m_ptr)
            static_cast<ID3D12Device*>(device->m_ptr)->Release();

        delete device;
    }
}
