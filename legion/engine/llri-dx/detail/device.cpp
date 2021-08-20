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
        result mapHRESULT(HRESULT value);

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
        output->m_currentCount = 0;
        output->m_type = desc.type;

        ID3D12CommandAllocator* allocator;
        const auto r = static_cast<ID3D12Device*>(m_ptr)->CreateCommandAllocator(internal::mapCommandGroupType(desc.type), IID_PPV_ARGS(&allocator));
        if (FAILED(r))
        {
            destroyCommandGroup(output);
            return internal::mapHRESULT(r);
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
}
