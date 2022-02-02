/**
 * @file instance.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-dx/directx.hpp>

namespace llri
{
    namespace internal
    {
        result enableDriverValidationEXT();
        result enableGPUValidationEXT();

        message_severity mapSeverity(D3D12_MESSAGE_SEVERITY sev)
        {
            switch (sev)
            {
            case D3D12_MESSAGE_SEVERITY_CORRUPTION:
                return message_severity::Corruption;
            case D3D12_MESSAGE_SEVERITY_ERROR:
                return message_severity::Error;
            case D3D12_MESSAGE_SEVERITY_WARNING:
                return message_severity::Warning;
            case D3D12_MESSAGE_SEVERITY_INFO:
                return message_severity::Info;
            case D3D12_MESSAGE_SEVERITY_MESSAGE:
                return message_severity::Verbose;
            }

            return message_severity::Info;
        }

        INT mapQueuePriority(queue_priority priority)
        {
            switch(priority)
            {
                case queue_priority::Normal:
                    return D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
                case queue_priority::High:
                    return D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
            }

            return 0;
        }

        D3D12_COMMAND_LIST_TYPE mapQueueType(queue_type type)
        {
            switch(type)
            {
                case queue_type::Graphics:
                    return D3D12_COMMAND_LIST_TYPE_DIRECT;
                case queue_type::Compute:
                    return D3D12_COMMAND_LIST_TYPE_COMPUTE;
                case queue_type::Transfer:
                    return D3D12_COMMAND_LIST_TYPE_COPY;
            }

            return D3D12_COMMAND_LIST_TYPE_DIRECT;
        }
    }

    namespace detail
    {
        result impl_createInstance(const instance_desc& desc, Instance** instance, bool enableImplementationMessagePolling)
        {
            directx::lazyInitializeDirectX();

            auto* output = new Instance();
            output->m_desc = desc;
            UINT factoryFlags = 0;

            for (size_t i = 0; i < desc.numExtensions; i++)
            {
                auto& extension = desc.extensions[i];
                result extensionCreateResult;

                switch (extension)
                {
                    case instance_extension::DriverValidation:
                    {
                        extensionCreateResult = internal::enableDriverValidationEXT();
                        if (extensionCreateResult == result::Success)
                            factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
                        break;
                    }
                    case instance_extension::GPUValidation:
                    {
                        extensionCreateResult = internal::enableGPUValidationEXT();
                        break;
                    }
                    case instance_extension::SurfaceWin32:
                    {
                        // do nothing, enabled by default
                        extensionCreateResult = result::Success;
                        break;
                    }
                    default:
                    {
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

            // DirectX creates validation callbacks upon device creation so we just need to store information about this right now.
            output->m_validationCallbackMessenger = nullptr;
            output->m_shouldConstructValidationCallbackMessenger = enableImplementationMessagePolling;

            // Attempt to create factory
            IDXGIFactory* factory = nullptr;
            HRESULT factoryCreateResult = directx::CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory));

            // DXGI_CREATE_FACTORY_DEBUG may not be a supported flag if the graphics tools aren't installed
            // so if this the previous call fails, use default factory flags
            if (FAILED(factoryCreateResult))
                factoryCreateResult = directx::CreateDXGIFactory2(0, IID_PPV_ARGS(&factory));

            // Check for failure
            if (FAILED(factoryCreateResult))
            {
                llri::destroyInstance(output);
                return directx::mapHRESULT(factoryCreateResult);
            }

            // Store factory and return result
            output->m_ptr = factory;
            *instance = output;
            return result::Success;
        }

        void impl_destroyInstance(Instance* instance)
        {
            for (auto& [ptr, adapter] : instance->m_cachedAdapters)
            {
                if (adapter->m_ptr)
                    static_cast<IDXGIAdapter*>(adapter->m_ptr)->Release();
                delete adapter;
            }

            ID3D12Debug1* debugGPU = nullptr;
            if (SUCCEEDED(directx::D3D12GetDebugInterface(IID_PPV_ARGS(&debugGPU))))
            {
                debugGPU->SetEnableGPUBasedValidation(false);
                debugGPU->Release();
            }

            if (instance->m_ptr)
                static_cast<IDXGIFactory*>(instance->m_ptr)->Release();
            delete instance;

#ifndef NDEBUG
            IDXGIDebug* debug;
            if (SUCCEEDED(directx::DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
            {
                debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
                debug->Release();
            }
#endif
        }

        void impl_pollAPIMessages(messenger_type* messenger)
        {
            if (messenger != nullptr)
            {
                auto* iq = static_cast<ID3D12InfoQueue*>(messenger);
                const auto numMsg = iq->GetNumStoredMessages();

                for (UINT64 i = 0; i < numMsg; ++i)
                {
                    SIZE_T messageLength = 0;
                    if (FAILED(iq->GetMessage(i, NULL, &messageLength)))
                        continue;

                    if (messageLength == 0)
                        continue;

                    auto* pMessage = static_cast<D3D12_MESSAGE*>(malloc(messageLength));
                    if (FAILED(iq->GetMessage(i, pMessage, &messageLength)))
                        continue;

                    if (pMessage->pDescription != nullptr)
                        detail::callUserCallback(internal::mapSeverity(pMessage->Severity), message_source::Implementation, pMessage->pDescription);

                    free(pMessage);
                }

                iq->ClearStoredMessages();
            }
        }
    }

    result Instance::impl_enumerateAdapters(std::vector<Adapter*>* adapters)
    {
        IDXGIAdapter* dxgiAdapter = nullptr;
        HRESULT result = 0;
        uint32_t i = 0;
        while (true)
        {
            result = static_cast<IDXGIFactory*>(m_ptr)->EnumAdapters(i, &dxgiAdapter);
            if (result == DXGI_ERROR_NOT_FOUND)
                break;

            // Ignore Microsoft Basic Render Driver
            DXGI_ADAPTER_DESC1 desc;
            static_cast<IDXGIAdapter1*>(dxgiAdapter)->GetDesc1(&desc); // DirectX casts should always be static
            const uint64_t luid = desc.AdapterLuid.LowPart + desc.AdapterLuid.HighPart;
            if (desc.Flags == DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                dxgiAdapter->Release();
                i++;
                continue;
            }

            // Ignore adapters incompatible with DX12
            HRESULT level12_0 = directx::D3D12CreateDevice(dxgiAdapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr);

            if (level12_0 == E_FAIL)
            {
                dxgiAdapter->Release();
                i++;
                continue;
            }

            if (m_cachedAdapters.find((void*)luid) != m_cachedAdapters.end())
            {
                // Re-assign pointer to found adapters
                m_cachedAdapters[(void*)luid]->m_ptr = dxgiAdapter;
                adapters->push_back(m_cachedAdapters[(void*)luid]);
            }
            else
            {
                Adapter* adapter = new Adapter();
                adapter->m_ptr = dxgiAdapter;
                adapter->m_instanceHandle = m_ptr;
                adapter->m_validationCallbackMessenger = m_validationCallbackMessenger;

                // Attempt to query node count
                ID3D12Device* device = nullptr;
                if (SUCCEEDED(directx::D3D12CreateDevice(dxgiAdapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device))))
                {
                    adapter->m_nodeCount = static_cast<uint8_t>(device->GetNodeCount());
                    device->Release();
                }

                m_cachedAdapters[(void*)luid] = adapter;
                adapters->push_back(adapter);
            }

            i++;
        }

        return result::Success;
    }

    result Instance::impl_createDevice(const device_desc& desc, Device** device)
    {
        const D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_0; // 12.0 is the bare minimum

        ID3D12Device* dx12Device = nullptr;
        HRESULT r = directx::D3D12CreateDevice(static_cast<IDXGIAdapter*>(desc.adapter->m_ptr), featureLevel, IID_PPV_ARGS(&dx12Device));
        if (FAILED(r))
            return directx::mapHRESULT(r);

        auto* output = new Device();
        output->m_desc = desc;
        output->m_adapter = desc.adapter;
        output->m_validationCallbackMessenger = m_validationCallbackMessenger;
        output->m_ptr = dx12Device;

        if (m_shouldConstructValidationCallbackMessenger)
        {
            ID3D12InfoQueue* iq = nullptr;
            if (SUCCEEDED(dx12Device->QueryInterface(IID_PPV_ARGS(&iq))))
                output->m_validationCallbackMessenger = iq;
        }

        std::vector<void*> queues(desc.adapter->m_nodeCount, nullptr);
        std::vector<Fence*> fences(desc.adapter->m_nodeCount, nullptr);
        for (size_t i = 0; i < desc.numQueues; i++)
        {
            queues.assign(queues.size(), nullptr);
            fences.assign(fences.size(), nullptr);

            auto& queueDesc = desc.queues[i];

            const INT priority = internal::mapQueuePriority(queueDesc.priority);
            const D3D12_COMMAND_LIST_TYPE type = internal::mapQueueType(queueDesc.type);

            for (size_t node = 0; node < desc.adapter->m_nodeCount; node++)
            {
                D3D12_COMMAND_QUEUE_DESC dx12QueueDesc { type, priority, D3D12_COMMAND_QUEUE_FLAG_NONE, 1u << static_cast<UINT>(node) };
                ID3D12CommandQueue* dx12Queue = nullptr;
                r = dx12Device->CreateCommandQueue(&dx12QueueDesc, IID_PPV_ARGS(&dx12Queue));
                if (FAILED(r))
                {
                    // the internal queue resources for this queue havent been added to the device yet so they must be destroyed manually
                    for (void* q : queues) { if (q) static_cast<ID3D12CommandQueue*>(q)->Release(); };
                    for (Fence* f : fences) { if (f) output->destroyFence(f); }

                    destroyDevice(output);
                    return directx::mapHRESULT(r);
                }

                queues[node] = dx12Queue;

                const result re = output->createFence(fence_flag_bits::None, &fences[node]);
                if (re != result::Success)
                {
                    // the internal queue resources for this queue havent been added to the device yet so they must be destroyed manually
                    for (void* q : queues) { if (q) static_cast<ID3D12CommandQueue*>(q)->Release(); };
                    for (Fence* f : fences) { if (f) output->destroyFence(f); }

                    destroyDevice(output);
                    return re;
                }
            }

            auto* queue = new Queue();
            queue->m_desc = queueDesc;
            queue->m_device = output;
            queue->m_ptrs = queues;
            queue->m_fences = fences;
            queue->m_validationCallbackMessenger = output->m_validationCallbackMessenger;

            switch(queueDesc.type)
            {
                case queue_type::Graphics:
                {
                    output->m_graphicsQueues.push_back(queue);
                    break;
                }
                case queue_type::Compute:
                {
                    output->m_computeQueues.push_back(queue);
                    break;
                }
                case queue_type::Transfer:
                {
                    output->m_transferQueues.push_back(queue);
                    break;
                }
            }
        }

        *device = output;
        return result::Success;
    }

    void Instance::impl_destroyDevice(Device* device)
    {
        for (auto* graphics : device->m_graphicsQueues)
        {
            for (size_t i = 0; i < graphics->m_ptrs.size(); i++)
            {
                if (graphics->m_ptrs[i])
                    static_cast<ID3D12CommandQueue*>(graphics->m_ptrs[i])->Release();

                if (graphics->m_fences[i])
                    device->destroyFence(graphics->m_fences[i]);
            }
            delete graphics;
        }

        for (auto* compute : device->m_computeQueues)
        {
            for (size_t i = 0; i < compute->m_ptrs.size(); i++)
            {
                if (compute->m_ptrs[i])
                    static_cast<ID3D12CommandQueue*>(compute->m_ptrs[i])->Release();

                if (compute->m_fences[i])
                    device->destroyFence(compute->m_fences[i]);
            }
            delete compute;
        }
        
        for (auto* transfer : device->m_transferQueues)
        {
            for (size_t i = 0; i < transfer->m_ptrs.size(); i++)
            {
                if (transfer->m_ptrs[i])
                    static_cast<ID3D12CommandQueue*>(transfer->m_ptrs[i])->Release();

                if (transfer->m_fences[i])
                    device->destroyFence(transfer->m_fences[i]);
            }
            delete transfer;
        }

        if (device->m_validationCallbackMessenger)
            static_cast<ID3D12InfoQueue*>(device->m_validationCallbackMessenger)->Release();

        if (device->m_ptr)
        {
#ifndef NDEBUG
            ID3D12DebugDevice* debugDevice;
            if (SUCCEEDED(static_cast<ID3D12Device*>(device->m_ptr)->QueryInterface(&debugDevice)))
            {
                debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
                debugDevice->Release();
            }
#endif
            static_cast<ID3D12Device*>(device->m_ptr)->Release();
        }

        delete device;
    }

        result Instance::impl_createSurfaceEXT(const surface_win32_desc_ext& desc, SurfaceEXT** surface)
    {
        // simply store the hwnd pointer for DX12
        auto* output = new SurfaceEXT();
        output->m_ptr = static_cast<void*>(desc.hwnd);
        *surface = output;
        return result::Success;
    }

    result Instance::impl_createSurfaceEXT(const surface_cocoa_desc_ext& desc, SurfaceEXT** surface)
    {
        return result::ErrorExtensionNotSupported;
    }

    result Instance::impl_createSurfaceEXT(const surface_xlib_desc_ext& desc, SurfaceEXT** surface)
    {
        return result::ErrorExtensionNotSupported;
    }

    result Instance::impl_createSurfaceEXT(const surface_xcb_desc_ext& desc, SurfaceEXT** surface)
    {
        return result::ErrorExtensionNotSupported;
    }

    void Instance::impl_destroySurfaceEXT(SurfaceEXT* surface)
    {
        delete surface;
    }
}
