/**
 * @file device.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-dx/directx.hpp>

namespace LLRI_NAMESPACE
{
    namespace internal
    {
        D3D12_COMMAND_LIST_TYPE mapCommandGroupType(queue_type type)
        {
            switch (type)
            {
                case queue_type::Graphics:
                    return D3D12_COMMAND_LIST_TYPE_DIRECT;
                case queue_type::Compute:
                    return D3D12_COMMAND_LIST_TYPE_COMPUTE;
                case queue_type::Transfer:
                    return D3D12_COMMAND_LIST_TYPE_COPY;
            }

            throw;
        }
    }

    result Device::impl_createCommandGroup(const command_group_desc& desc, CommandGroup** cmdGroup) const
    {
        auto* output = new CommandGroup();
        output->m_deviceHandle = m_ptr;
        output->m_deviceFunctionTable = m_functionTable;
        output->m_validationCallback = m_validationCallback;
        output->m_validationCallbackMessenger = m_validationCallbackMessenger;
        output->m_maxCount = desc.count;
        output->m_type = desc.type;

        ID3D12CommandAllocator* allocator;
        const auto r = static_cast<ID3D12Device*>(m_ptr)->CreateCommandAllocator(internal::mapCommandGroupType(desc.type), IID_PPV_ARGS(&allocator));
        if (FAILED(r))
        {
            destroyCommandGroup(output);
            return directx::mapHRESULT(r);
        }

        output->m_ptr = allocator;
        *cmdGroup = output;
        return result::Success;
    }

    void Device::impl_destroyCommandGroup(CommandGroup* cmdGroup) const
    {
        if (!cmdGroup)
            return;

        if (cmdGroup->m_ptr)
            static_cast<ID3D12CommandAllocator*>(cmdGroup->m_ptr)->Release();

        delete cmdGroup;
    }

    result Device::impl_allocateCommandList(const command_list_alloc_desc& desc, CommandList** cmdList) const
    {
        ID3D12GraphicsCommandList* dx12CommandList = nullptr;

        const auto type = desc.usage == command_list_usage::Direct ?
                      internal::mapCommandGroupType(desc.group->m_type) :
                      D3D12_COMMAND_LIST_TYPE_BUNDLE;

        const HRESULT r = static_cast<ID3D12Device*>(m_ptr)
            ->CreateCommandList(0,
                type,
                static_cast<ID3D12CommandAllocator*>(desc.group->m_ptr),
                nullptr,
                IID_PPV_ARGS(&dx12CommandList));

        if (FAILED(r))
            return directx::mapHRESULT(r);

        //Dx12 command lists are opened by default, close to comply with our system
        dx12CommandList->Close();

        auto* output = new CommandList();
        output->m_ptr = dx12CommandList;
        output->m_groupHandle = desc.group->m_ptr;

        output->m_deviceHandle = m_ptr;
        output->m_deviceFunctionTable = m_functionTable;

        output->m_usage = desc.usage;
        output->m_state = command_list_state::Empty;

        output->m_validationCallback = m_validationCallback;
        output->m_validationCallbackMessenger = m_validationCallbackMessenger;

        desc.group->m_cmdLists.push_back(output);

        *cmdList = output;
        return result::Success;
    }
    
    result Device::impl_allocateCommandLists(const command_list_alloc_desc& desc, uint8_t count, std::vector<CommandList*>* cmdLists) const
    {
        std::vector<CommandList*> output;

        const auto type = desc.usage == command_list_usage::Direct ?
                              internal::mapCommandGroupType(desc.group->m_type) :
                              D3D12_COMMAND_LIST_TYPE_BUNDLE;

        for (uint8_t i = 0; i < count; i++)
        {
            ID3D12GraphicsCommandList* dx12CommandList = nullptr;

            const HRESULT r = static_cast<ID3D12Device*>(m_ptr)
                ->CreateCommandList(0,
                    type,
                    static_cast<ID3D12CommandAllocator*>(desc.group->m_ptr),
                    nullptr,
                    IID_PPV_ARGS(&dx12CommandList));

            if (FAILED(r))
            {
                //Free already allocated command lists
                freeCommandLists(desc.group, static_cast<uint8_t>(output.size()), output.data());

                return directx::mapHRESULT(r);
            }

            //Dx12 command lists are opened by default, close to comply with our system
            dx12CommandList->Close();

            auto* cmdList = new CommandList();
            cmdList->m_ptr = dx12CommandList;
            cmdList->m_groupHandle = desc.group->m_ptr;

            cmdList->m_deviceHandle = m_ptr;
            cmdList->m_deviceFunctionTable = m_functionTable;

            cmdList->m_usage = desc.usage;
            cmdList->m_state = command_list_state::Empty;

            cmdList->m_validationCallback = m_validationCallback;
            cmdList->m_validationCallbackMessenger = m_validationCallbackMessenger;

            desc.group->m_cmdLists.push_back(cmdList);
            output.push_back(cmdList);
        }

        *cmdLists = output;
        return result::Success;
    }

    result Device::impl_freeCommandList(CommandGroup* group, CommandList* cmdList) const
    {
        //Free internal pointer
        static_cast<IUnknown*>(cmdList->m_ptr)->Release();

        //Remove from commandlist list
        group->m_cmdLists.remove(cmdList);

        //Delete wrapper
        delete cmdList;
        return result::Success;
    }

    result Device::impl_freeCommandLists(CommandGroup* group, uint8_t numCommandLists, CommandList** cmdLists) const
    {
        for (uint8_t i = 0; i < numCommandLists; i++)
        {
            //Free internal pointer
            static_cast<IUnknown*>(cmdLists[i]->m_ptr)->Release();

            //Remove from commandlist list
            group->m_cmdLists.remove(cmdLists[i]);

            //Delete wrapper
            delete cmdLists[i];
        }

        return result::Success;
    }
}
