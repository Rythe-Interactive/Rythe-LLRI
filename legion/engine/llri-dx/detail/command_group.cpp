/**
 * @file command_group.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-dx/directx.hpp>

namespace llri
{
    result CommandGroup::impl_reset()
    {
        const auto r = static_cast<ID3D12CommandAllocator*>(m_ptr)->Reset();
        if (FAILED(r))
            return directx::mapHRESULT(r);

        for (auto* cmdList : m_cmdLists)
            cmdList->m_state = command_list_state::Empty;

        return result::Success;
    }

    result CommandGroup::impl_allocate(const command_list_alloc_desc& desc, CommandList** cmdList)
    {
        ID3D12GraphicsCommandList* dx12CommandList = nullptr;

        const auto type = (desc.usage == command_list_usage::Direct ?
                      directx::mapCommandGroupType(m_type) :
                      D3D12_COMMAND_LIST_TYPE_BUNDLE);

        auto* allocator = (desc.usage == command_list_usage::Direct ?
            m_ptr : m_indirectPtr);

        const HRESULT r = static_cast<ID3D12Device*>(m_device->m_ptr)
            ->CreateCommandList(desc.nodeMask,
                type,
                static_cast<ID3D12CommandAllocator*>(allocator),
                nullptr,
                IID_PPV_ARGS(&dx12CommandList));

        if (FAILED(r))
            return directx::mapHRESULT(r);

        // Dx12 command lists are opened by default, close to comply with our system
        dx12CommandList->Close();

        auto* output = new CommandList();
        output->m_ptr = dx12CommandList;
        output->m_group = this;

        output->m_deviceHandle = m_device->m_ptr;
        output->m_deviceFunctionTable = m_deviceFunctionTable;

        output->m_desc = desc;
        output->m_state = command_list_state::Empty;

        output->m_validationCallbackMessenger = m_validationCallbackMessenger;

        m_cmdLists.emplace(output);

        *cmdList = output;
        return result::Success;
    }
    
    result CommandGroup::impl_allocate(const command_list_alloc_desc& desc, uint8_t count, std::vector<CommandList*>* cmdLists)
    {
        const auto type = (desc.usage == command_list_usage::Direct ?
                              directx::mapCommandGroupType(m_type) :
                              D3D12_COMMAND_LIST_TYPE_BUNDLE);

        auto* allocator = (desc.usage == command_list_usage::Direct ?
            static_cast<ID3D12CommandAllocator*>(m_ptr) :
            static_cast<ID3D12CommandAllocator*>(m_indirectPtr));

        cmdLists->reserve(count);

        for (uint8_t i = 0; i < count; i++)
        {
            ID3D12GraphicsCommandList* dx12CommandList = nullptr;

            const HRESULT r = static_cast<ID3D12Device*>(m_device->m_ptr)
                ->CreateCommandList(desc.nodeMask,
                    type,
                    allocator,
                    nullptr,
                    IID_PPV_ARGS(&dx12CommandList));

            if (FAILED(r))
            {
                // Free already allocated command lists
                this->free(static_cast<uint8_t>(cmdLists->size()), cmdLists->data());
                cmdLists->clear();
                return directx::mapHRESULT(r);
            }

            // Dx12 command lists are opened by default, close to comply with our system
            dx12CommandList->Close();

            auto* cmdList = new CommandList();
            cmdList->m_ptr = dx12CommandList;
            cmdList->m_group = this;

            cmdList->m_deviceHandle = m_device->m_ptr;
            cmdList->m_deviceFunctionTable = m_deviceFunctionTable;

            cmdList->m_desc = desc;
            cmdList->m_state = command_list_state::Empty;

            cmdList->m_validationCallbackMessenger = m_validationCallbackMessenger;

            m_cmdLists.emplace(cmdList);
            cmdLists->push_back(cmdList);
        }

        return result::Success;
    }

    result CommandGroup::impl_free(CommandList* cmdList)
    {
        // Free internal pointer
        static_cast<IUnknown*>(cmdList->m_ptr)->Release();

        // Remove from commandlist list
        m_cmdLists.erase(cmdList);

        // Delete wrapper
        delete cmdList;
        return result::Success;
    }

    result CommandGroup::impl_free(uint8_t numCommandLists, CommandList** cmdLists)
    {
        for (size_t i = 0; i < numCommandLists; i++)
            impl_free(cmdLists[i]);

        return result::Success;
    }
}
