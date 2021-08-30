/**
 * @file device.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-dx/directx.hpp>

namespace LLRI_NAMESPACE
{
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
        auto r = static_cast<ID3D12Device*>(m_ptr)->CreateCommandAllocator(directx::mapCommandGroupType(desc.type), IID_PPV_ARGS(&allocator));
        if (FAILED(r))
        {
            destroyCommandGroup(output);
            return directx::mapHRESULT(r);
        }
        output->m_ptr = allocator;

        ID3D12CommandAllocator* indirectAllocator;
        r = static_cast<ID3D12Device*>(m_ptr)->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(&indirectAllocator));
        if (FAILED(r))
        {
            destroyCommandGroup(output);
            return directx::mapHRESULT(r);
        }
        output->m_indirectPtr = indirectAllocator;

        *cmdGroup = output;
        return result::Success;
    }

    void Device::impl_destroyCommandGroup(CommandGroup* cmdGroup) const
    {
        if (!cmdGroup)
            return;

        if (cmdGroup->m_ptr)
            static_cast<ID3D12CommandAllocator*>(cmdGroup->m_ptr)->Release();

        if (cmdGroup->m_indirectPtr)
            static_cast<ID3D12CommandAllocator*>(cmdGroup->m_indirectPtr)->Release();

        delete cmdGroup;
    }
}
