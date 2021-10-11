/**
 * @file device.inl
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> //Recursive include technically not necessary but helps with intellisense
#include <set>

namespace LLRI_NAMESPACE
{
    inline result Device::queryQueue(queue_type type, uint8_t index, Queue** queue)
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (queue == nullptr)
        {
            detail::apiError("Device::queryQueue()", result::ErrorInvalidUsage, "the passed queue parameter must not be nullptr.");
            return result::ErrorInvalidUsage;
        }
#endif

        *queue = nullptr;

#ifndef LLRI_DISABLE_VALIDATION
        if (type > queue_type::MaxEnum)
        {
            detail::apiError("Device::queryQueue()", result::ErrorInvalidUsage, "the passed type parameter " + std::to_string((uint8_t)type) + " is not a valid queue_type value.");
            return result::ErrorInvalidUsage;
        }
#endif

        std::vector<Queue*>* queues = nullptr;
        switch(type)
        {
            case queue_type::Graphics:
            {
                queues = &m_graphicsQueues;
                break;
            }
            case queue_type::Compute:
            {
                queues = &m_computeQueues;
                break;
            }
            case queue_type::Transfer:
            {
                queues = &m_transferQueues;
                break;
            }
        }

#ifndef LLRI_DISABLE_VALIDATION
        if (index >= static_cast<uint8_t>(queues->size()))
        {
            detail::apiError("Device::queryQueue()", result::ErrorInvalidUsage, "the passed index parameter " + std::to_string(index) + " is not smaller than the number of created queues (" + std::to_string(queues->size()) + ") of type " + to_string(type) + ".");
            return result::ErrorInvalidUsage;
        }
#endif

        *queue = queues->at(index);
        return result::Success;
    }

    inline uint8_t Device::queryQueueCount(queue_type type)
    {
        switch (type)
        {
            case queue_type::Graphics:
                return m_graphicsQueues.size();
            case queue_type::Compute:
                return m_computeQueues.size();
            case queue_type::Transfer:
                return m_transferQueues.size();
        }

        return 0;
    }

    inline result Device::createCommandGroup(const command_group_desc& desc, CommandGroup** cmdGroup)
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (cmdGroup == nullptr)
        {
            detail::apiError("Device::createCommandGroup()", result::ErrorInvalidUsage, "the passed cmdGroup parameter must not be nullptr.");
            return result::ErrorInvalidUsage;
        }
#endif

        *cmdGroup = nullptr;

#ifndef LLRI_DISABLE_VALIDATION
        if (desc.type > queue_type::MaxEnum)
        {
            detail::apiError("Device::createCommandGroup()", result::ErrorInvalidUsage, "desc.type was not a valid queue_type enum value.");
            return result::ErrorInvalidUsage;
        }

        if (desc.count == 0)
        {
            detail::apiError("Device::createCommandGroup()", result::ErrorInvalidUsage, "because desc.count was 0");
            return result::ErrorInvalidUsage;
        }

        const uint8_t availableQueueCount = queryQueueCount(desc.type);
        if (availableQueueCount == 0)
        {
            detail::apiError("Device::createCommandGroup()", result::ErrorInvalidUsage, "the Device has no queues for the passed command_group_desc::type.");
            return result::ErrorInvalidUsage;
        }
#endif

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_createCommandGroup(desc, cmdGroup);
        detail::impl_pollAPIMessages(m_validationCallbackMessenger);
        return r;
#else
        return impl_createCommandGroup(desc, cmdGroup);
#endif
    }

    inline void Device::destroyCommandGroup(CommandGroup* cmdGroup)
    {
        if (!cmdGroup)
            return;

        if (cmdGroup->m_ptr)
        {
            std::vector<CommandList*> cmdLists;
            cmdLists.reserve(cmdGroup->m_cmdLists.size());
            for (auto element : cmdGroup->m_cmdLists)
                cmdLists.push_back(element);
            cmdGroup->free(static_cast<uint8_t>(cmdLists.size()), cmdLists.data());
        }

        impl_destroyCommandGroup(cmdGroup);

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        detail::impl_pollAPIMessages(m_validationCallbackMessenger);
#endif
    }

    inline result Device::createFence(fence_flags flags, Fence** fence)
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (fence == nullptr)
        {
            detail::apiError("Device::createFence()", result::ErrorInvalidUsage, "the passed fence parameter must not be nullptr.");
            return result::ErrorInvalidUsage;
        }
#endif

        *fence = nullptr;

#ifndef LLRI_DISABLE_VALIDATION
        const std::unordered_set<fence_flags> supportedFlags = {
            fence_flag_bits::None,
            fence_flag_bits::Signaled
        };

        if (supportedFlags.find(flags) == supportedFlags.end())
        {
            detail::apiError("Device::createFence()", result::ErrorInvalidUsage, "the flags value " + std::to_string(flags) + "was not a supported combination of fence_flags.");
            return result::ErrorInvalidUsage;
        }
#endif

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_createFence(flags, fence);
        detail::impl_pollAPIMessages(m_validationCallbackMessenger);
        return r;
#else
        return impl_createFence(flags, fence);
#endif
    }

    inline void Device::destroyFence(Fence* fence)
    {
        if (!fence)
            return;

        impl_destroyFence(fence);

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        detail::impl_pollAPIMessages(m_validationCallbackMessenger);
#endif
    }

    inline result Device::waitFences(uint32_t numFences, Fence** fences, uint64_t timeout)
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (numFences == 0)
        {
            detail::apiError("Device::waitFences()", result::ErrorInvalidUsage, "because numFences was 0.");
            return result::ErrorInvalidUsage;
        }

        if (fences == nullptr)
        {
            detail::apiError("Device::waitFences()", result::ErrorInvalidUsage, "fences was nullptr.");
            return result::ErrorInvalidUsage;
        }

        for (size_t i = 0; i < numFences; i++)
        {
            if (fences[i] == nullptr)
            {
                detail::apiError("Device::waitFences()", result::ErrorInvalidUsage, "fences[" + std::to_string(i) + "] was nullptr.");
                return result::ErrorInvalidUsage;
            }

            if (!fences[i]->m_signaled)
            {
                detail::apiError("Device::waitFences()", result::ErrorNotSignaled, "fences[" + std::to_string(i) + "] was not signaled");
                return result::ErrorNotSignaled;
            }
        }
#endif

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_waitFences(numFences, fences, timeout);
        detail::impl_pollAPIMessages(m_validationCallbackMessenger);
#else
        const auto r = impl_waitFences(numFences, fences, timeout)
#endif

        if (r == result::Success)
        {
            for (size_t i = 0; i < numFences; i++)
                fences[i]->m_signaled = false;
        }
        return r;
    }

    inline result Device::waitFence(Fence* fence, uint64_t timeout)
    {
        return waitFences(1, &fence, timeout);
    }

    inline result Device::createSemaphore(Semaphore** semaphore)
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (semaphore == nullptr)
        {
            detail::apiError("Device::createSemaphore()", result::ErrorInvalidUsage, "because semaphore was nullptr.");
            return result::ErrorInvalidUsage;
        }
#endif

        *semaphore = nullptr;
        
#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_createSemaphore(semaphore);
        detail::impl_pollAPIMessages(m_validationCallbackMessenger);
        return r;
#else
        return impl_createSemaphore(semaphore);
#endif
    }

    inline void Device::destroySemaphore(Semaphore* semaphore)
    {
        if (!semaphore)
            return;

        impl_destroySemaphore(semaphore);

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        detail::impl_pollAPIMessages(m_validationCallbackMessenger);
#endif
    }

    namespace detail
    {
        constexpr size_t getFormatSizeInBytes(format f) noexcept
        {
            switch (f)
            {
                case format::Undefined: return 0;
                case format::R8UNorm:
                case format::R8Norm: 
                case format::R8UInt: 
                case format::R8Int:
                    return 1;
                case format::RG8UNorm:
                case format::RG8Norm:
                case format::RG8UInt:
                case format::RG8Int:
                    return 2;
                case format::RGBA8UNorm:
                case format::RGBA8Norm:
                case format::RGBA8UInt:
                case format::RGBA8Int:
                case format::RGBA8sRGB:
                case format::BGRA8UNorm:
                case format::BGRA8sRGB:
                case format::RGB10A2UNorm:
                case format::RGB10A2UInt:
                    return 4;
                case format::R16UNorm:
                case format::R16Norm:
                case format::R16UInt:
                case format::R16Int:
                case format::R16Float:
                    return 2;
                case format::RG16UNorm:
                case format::RG16Norm:
                case format::RG16UInt:
                case format::RG16Int:
                case format::RG16Float:
                    return 4;
                case format::RGBA16UNorm:
                case format::RGBA16Norm:
                case format::RGBA16UInt:
                case format::RGBA16Int:
                case format::RGBA16Float:
                    return 8;
                case format::R32UInt:
                case format::R32Int:
                case format::R32Float:
                    return 4;
                case format::RG32UInt:
                case format::RG32Int:
                case format::RG32Float:
                    return 8;
                case format::RGB32UInt:
                case format::RGB32Int:
                case format::RGB32Float:
                case format::RGBA32UInt:
                case format::RGBA32Int:
                case format::RGBA32Float:
                    return 16;
                case format::D16UNorm:
                    return 2;
                case format::D24UNormS8UInt:
                    return 4;
                case format::D32Float:
                    return 4;
                case format::D32FloatS8X24UInt:
                    return 8;
            }

            return 0;
        }
    }

    inline result Device::createResource(const resource_desc& desc, Resource** resource)
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (resource == nullptr)
        {
            detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "resource was nullptr.");
            return result::ErrorInvalidUsage;
        }
#endif

        *resource = nullptr;

#ifndef LLRI_DISABLE_VALIDATION
        // convert zero to one for validation on nodemasks
        uint32_t createNodeMask = desc.createNodeMask;
        if (createNodeMask == 0)
            createNodeMask = 1;

        uint32_t visibleNodeMask = desc.visibleNodeMask;
        if (visibleNodeMask == 0)
            visibleNodeMask = 1;

        // In resource creation, there are a lot of combinations that can be incorrect
        // checks for these combinations can get confusing,
        // our rule will be to only check against previously checked variables. e.g. if we check desc.type first, we only need to check that its valid,
        // but if we check desc.usage we must check if its valid with desc.type

        //determines if the node mask is not a power of two -> if it isn't then multiple bits are set
        if ((createNodeMask & (createNodeMask - 1)) != 0)
        {
            detail::apiError("Device::createResource()", result::ErrorInvalidNodeMask, "desc.createNodeMask " + std::to_string(createNodeMask) + " has multiple bits set.");
            return result::ErrorInvalidNodeMask;
        }

        if (createNodeMask >= (1 << m_adapter->queryNodeCount()))
        {
            detail::apiError("Device::createResource()", result::ErrorInvalidNodeMask, "desc.createNodeMask " + std::to_string(createNodeMask) + " has a bit set that is >= (1 << Adapter::queryNodeCount()).");
            return result::ErrorInvalidNodeMask;
        }

        if ((visibleNodeMask & createNodeMask) != createNodeMask)
        {
            detail::apiError("Device::createResource()", result::ErrorInvalidNodeMask, "desc.visibleNodeMask doesn't have at least the same bit set as desc.createNodeMask.");
            return result::ErrorInvalidNodeMask;
        }

        const uint32_t maxVisibleNodeMask = (1 << m_adapter->queryNodeCount()) - 1;
        if (visibleNodeMask > maxVisibleNodeMask)
        {
            detail::apiError("Device::createResource()", result::ErrorInvalidNodeMask, "desc.visibleNodeMask has a bit set that is >= 1 << Adapter::queryNodeCount().");
            return result::ErrorInvalidNodeMask;
        }

        // desc.type is a valid value
        if (desc.type > resource_type::MaxEnum)
        {
            detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.type (" + std::to_string(static_cast<uint8_t>(desc.type)) + ") was not set to a valid enum value.");
            return result::ErrorInvalidUsage;
        }

        bool isTexture = desc.type == resource_type::Texture1D || desc.type == resource_type::Texture2D || desc.type == resource_type::Texture3D;

        // desc.usage is a valid value
        constexpr resource_usage_flags maxFlags = resource_usage_flag_bits::TransferSrc |
            resource_usage_flag_bits::TransferDst |
            resource_usage_flag_bits::Sampled |
            resource_usage_flag_bits::ShaderWrite |
            resource_usage_flag_bits::ColorAttachment |
            resource_usage_flag_bits::DepthStencilAttachment |
            resource_usage_flag_bits::DenyShaderResource;
        if (desc.usage > maxFlags)
        {
            detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.usage (" + std::to_string(static_cast<uint32_t>(desc.usage.value)) + ") is not a valid combination of resource_usage_flag_bits values");
            return result::ErrorInvalidUsage;
        }

        // desc.usage combinations
        if (desc.usage.contains(resource_usage_flag_bits::DenyShaderResource))
        {
            // DenyShaderResource **must** also have DepthStencilAttachment
            if (!desc.usage.contains(resource_usage_flag_bits::DepthStencilAttachment))
            {
                detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.usage (" + to_string(desc.usage) + ") has the DenyShaderResource bit set but does not have the DepthStencilAttachment bit set.");
                return result::ErrorInvalidUsage;
            }

            // and it **can not** have any shader related flags
            constexpr resource_usage_flags validUsage = resource_usage_flag_bits::DenyShaderResource | resource_usage_flag_bits::DepthStencilAttachment |
                resource_usage_flag_bits::TransferSrc | resource_usage_flag_bits::TransferDst;

            if (!validUsage.contains(desc.usage.value))
            {
                detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.usage ( " + to_string(desc.usage) + ") has the DenyShaderResource bit set but it has shader related usage flags set. Allowed flags are: " + to_string(validUsage));
                return result::ErrorInvalidUsage;
            }
        }

        // desc.usage against desc.type
        switch(desc.type)
        {
            case resource_type::Buffer:
            {
                constexpr resource_usage_flags validUsage = resource_usage_flag_bits::TransferSrc | resource_usage_flag_bits::TransferDst | resource_usage_flag_bits::ShaderWrite;
                if (!validUsage.contains(desc.usage.value))
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.type is Buffer but desc.usage has invalid resource_usage_flag_bits set. Valid flag bits for this type are: " + to_string(validUsage));
                    return result::ErrorInvalidUsage;
                }
                break;
            }
            case resource_type::Texture1D:
            case resource_type::Texture2D:
            case resource_type::Texture3D:
                break; // textures currently support all resource usage flags
        }

        // desc.memoryType is a valid value
        if (desc.memoryType > memory_type::MaxEnum)
        {
            detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.memoryType (" + std::to_string(static_cast<uint8_t>(desc.memoryType)) + " is not a valid memory_type value.");
            return result::ErrorInvalidUsage;
        }

        // desc.memoryType against desc.type
        if (desc.type != resource_type::Buffer && desc.memoryType != memory_type::Local)
        {
            detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "if desc.type is not Buffer then desc.memoryType **must** be set to Local.");
            return result::ErrorInvalidUsage;
        }

        // desc.memoryType against desc.usage
        switch(desc.memoryType)
        {
            case memory_type::Local:
                // local currently supports all flags
                break;
            case memory_type::Upload:
            {
                constexpr auto invalidFlags = resource_usage_flag_bits::ShaderWrite | resource_usage_flag_bits::ColorAttachment | resource_usage_flag_bits::DepthStencilAttachment | resource_usage_flag_bits::DenyShaderResource;

                if (desc.usage.any(invalidFlags))
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.memoryType is memory_type::Upload but desc.usage has one of the following flags set: " + to_string(invalidFlags));
                    return result::ErrorInvalidUsage;
                }
                break;
            }
            case memory_type::Read:
            {
                constexpr auto invalidFlags = resource_usage_flag_bits::ShaderWrite | resource_usage_flag_bits::ColorAttachment | resource_usage_flag_bits::DepthStencilAttachment | resource_usage_flag_bits::DenyShaderResource;

                if (desc.usage.any(invalidFlags))
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.memoryType is memory_type::Read but desc.usage has one of the following flags set: " + to_string(invalidFlags));
                    return result::ErrorInvalidUsage;
                }
                break;
            }
        }

        // desc.initialState is a valid value
        if (desc.initialState > resource_state::MaxEnum)
        {
            detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.initialState (" + std::to_string(static_cast<uint8_t>(desc.initialState)) + ") is not a valid resource_state value.");
            return result::ErrorInvalidUsage;
        }

        // desc.initialState against desc.type
        switch(desc.type)
        {
            case resource_type::Buffer:
            {
                const std::set<resource_state> validStates { resource_state::General, resource_state::Upload, resource_state::ShaderReadOnly, resource_state::ShaderReadWrite, resource_state::TransferSrc, resource_state::TransferDst, resource_state::VertexBuffer, resource_state::IndexBuffer, resource_state::ConstantBuffer };
                if (validStates.find(desc.initialState) == validStates.end())
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.type was set to Buffer but desc.initialState wasn't one of the following states: General, Upload, ShaderReadOnly, ShaderReadWrite, TransferSrc, TransferDst, VertexBuffer, IndexBuffer, ConstantBuffer.");
                    return result::ErrorInvalidUsage;
                }
                break;
            }
            case resource_type::Texture1D:
            case resource_type::Texture2D:
            case resource_type::Texture3D:
            {
                const std::set<resource_state> validStates { resource_state::General, resource_state::Upload, resource_state::ColorAttachment, resource_state::DepthStencilAttachment, resource_state::DepthStencilAttachmentReadOnly, resource_state::ShaderReadOnly, resource_state::ShaderReadWrite, resource_state::TransferSrc, resource_state::TransferDst };
                if (validStates.find(desc.initialState) == validStates.end())
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.type was set to Texture1D, Texture2D or Texture3D but desc.initialState wasn't one of the following states: General, Upload, ColorAttachment, DepthStencilAttachmentReadWrite, DepthStencilAttachmentReadOnly, ShaderReadOnly, ShaderReadWrite, TransferSrc, TransferDst.");
                    return result::ErrorInvalidUsage;
                }
                break;
            }
        }

        // desc.initialState against desc.usage
        switch(desc.initialState)
        {
            case resource_state::General: break;
            case resource_state::ShaderReadOnly: break;
            case resource_state::VertexBuffer: break;
            case resource_state::IndexBuffer: break;
            case resource_state::ConstantBuffer: break;

            case resource_state::Upload:
            {
                if (desc.usage.contains(resource_usage_flag_bits::ShaderWrite))
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.initialState was set to Upload but also has resource_usage_flag_bits::ShaderWrite set.");
                    return result::ErrorInvalidUsage;
                }
                break;   
            }
            case resource_state::ColorAttachment:
            {
                if (!desc.usage.contains(resource_usage_flag_bits::ColorAttachment))
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.initialState was set to ColorAttachment but desc.usage doesn't have the resource_usage_flag_bits::ColorAttachment bit set.");
                    return result::ErrorInvalidUsage;
                }
                break;   
            }
            case resource_state::DepthStencilAttachment:
            case resource_state::DepthStencilAttachmentReadOnly:
            {
                if (!desc.usage.contains(resource_usage_flag_bits::DepthStencilAttachment))
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.initialState was set to DepthStencilAttachment or DepthStencilAttachmentReadOnly, but desc.usage doesn't have the resource_usage_flag_bits::DepthStencilAttachment bit set.");
                    return result::ErrorInvalidUsage;
                }
                break;
            }
            case resource_state::ShaderReadWrite:
            {
                if (!desc.usage.contains(resource_usage_flag_bits::ShaderWrite))
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.initialState was set to ShaderReadWrite but desc.usage doesn't have the resource_usage_flag_bits::ShaderWrite bit set.");
                    return result::ErrorInvalidUsage;
                }
                break;
            }
            case resource_state::TransferSrc:
            {
                if (!desc.usage.contains(resource_usage_flag_bits::TransferSrc))
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.initialState was set to TransferSrc but desc.usage doesn't have the resource_usage_flag_bits::TransferSrc bit set.");
                    return result::ErrorInvalidUsage;
                }
                break;
            }
            case resource_state::TransferDst:
            {
                if (!desc.usage.contains(resource_usage_flag_bits::TransferDst))
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.initialState was set to TransferDst but desc.usage doesn't have the resource_usage_flag_bits::TransferDst bit set.");
                    return result::ErrorInvalidUsage;
                }
                break;
            }
        }

        // desc.initialState against desc.memoryType
        switch(desc.memoryType)
        {
            case memory_type::Local:
            {
                if (desc.initialState == resource_state::Upload)
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.memoryType was set to Local but desc.initialState was set to Upload.");
                    return result::ErrorInvalidUsage;
                }
                break;
            }
            case memory_type::Upload:
            {
                if (desc.initialState != resource_state::Upload)
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.memoryType was set to Upload but desc.initialState was not set to resource_state::Upload.");
                    return result::ErrorInvalidUsage;
                }
                break;
            }
            case memory_type::Read:
            {
                if (desc.initialState != resource_state::TransferDst)
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.memoryType was set to Read but desc.initialState was not set to resource_state::TransferDst");
                    return result::ErrorInvalidUsage;
                }
                break;
            }
        }

        // desc.width is a valid value
        if (desc.width == 0)
        {
            detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.width was not at least 1.");
            return result::ErrorInvalidUsage;
        }

        if (desc.width > 16348)
        {
            detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.width was more than 16348.");
            return result::ErrorInvalidUsage;
        }

        // width, height, depth against desc.type
        switch(desc.type)
        {
            case resource_type::Buffer:
            {
                if (desc.height > 1)
                    detail::apiWarning("Device::createResource()", "desc.type was resource_type::Buffer but desc.height was more than 1. This value will be ignored internally but this may be a user error. The size of the created resource will only be determined by desc.width.");

                if (desc.depthOrArrayLayers > 1)
                    detail::apiWarning("Device::createResource()", "desc.type was resource_type::Buffer but desc.depthOrArrayLayers was more than 1. This value will be ignored internally but this may be a user error. The size of the created resource will only be determined by desc.width.");
                break;
            }
            case resource_type::Texture1D:
            {
                if (desc.height != 1)
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.type was resource_type::Texture1D but desc.height was not 1.");
                    return result::ErrorInvalidUsage;
                }

                if (desc.depthOrArrayLayers < 1)
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.type was resource_type::Texture1D but desc.depthOrArrayLayers was not at least 1.");
                    return result::ErrorInvalidUsage;
                }
                break;
            }
            case resource_type::Texture2D:
            {
                if (desc.height < 1)
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.type was resource_type::Texture2D but desc.height was not at least 1.");
                    return result::ErrorInvalidUsage;
                }

                if (desc.depthOrArrayLayers < 1)
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.type was resource_type::Texture2D but desc.depthOrArrayLayers was not at least 1.");
                    return result::ErrorInvalidUsage;
                }
                break;
            }
            case resource_type::Texture3D:
            {
                if (desc.height < 1)
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.type was resource_type::Texture3D but desc.height was not at least 1.");
                    return result::ErrorInvalidUsage;
                }

                if (desc.depthOrArrayLayers < 1)
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.type was resource_type::Texture3D but desc.depthOrArrayLayers was not at least 1.");
                    return result::ErrorInvalidUsage;
                }
                break;
            }
        }

        // width, height, depth against desc.usage
        // no current checks

        // width, height, depth against desc.memoryType
        // no current checks

        // width, height, depth against desc.initialState
        // no current checks

        // desc.mipLevels is a valid value
        if (isTexture)
        {
            if (desc.height > 2048)
            {
                detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.height was more than 16348.");
                return result::ErrorInvalidUsage;
            }

            if (desc.depthOrArrayLayers > 16348)
            {
                detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.depthOrArrayLayers was more than 2048.");
                return result::ErrorInvalidUsage;
            }

            if (desc.mipLevels < 1)
            {
                detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.mipLevels was not at least 1.");
                return result::ErrorInvalidUsage;
            }

            if (desc.width == 1 && desc.mipLevels > 1)
            {
                detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.width was 1 and desc.mipLevels was not exactly 1");
                return result::ErrorInvalidUsage;
            }

            // desc.sampleCount is a valid value
            if (desc.sampleCount > sample_count::MaxEnum)
            {
                detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.sampleCount was not a valid enum value.");
                return result::ErrorInvalidUsage;
            }

            // desc.sampleCount against desc.type
            if (desc.sampleCount != sample_count::Count1 && desc.type != resource_type::Texture2D)
            {
                detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "if desc.type is not resource_type::Texture2D then desc.sampleCount must be Count1.");
                return result::ErrorInvalidUsage;
            }

            // desc.sampleCount against desc.usage
            if (desc.usage.contains(resource_usage_flag_bits::ShaderWrite) && desc.sampleCount > sample_count::Count1)
            {
                detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.usage had the ShaderWrite bit set but desc.sampleCount was not Count1.");
                return result::ErrorInvalidUsage;
            }

            if (desc.sampleCount > sample_count::Count1 && (!desc.usage.contains(resource_usage_flag_bits::ColorAttachment) && !desc.usage.contains(resource_usage_flag_bits::DepthStencilAttachment)))
            {
                detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "if desc.sampleCount is more than Count1 then desc.usage must have the ColorAttachment or DepthStencilAttachment bit set.");
                return result::ErrorInvalidUsage;
            }

            // desc.sampleCount against desc.mipLevels
            if (desc.mipLevels > 1 && desc.sampleCount > sample_count::Count1)
            {
                detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "if desc.mipLevels is more than 1 then desc.sampleCount **must** be sample_count::Count1");
                return result::ErrorInvalidUsage;
            }

            if (desc.mipLevels > 1 && desc.width < pow(2, desc.mipLevels))
            {
                detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "if desc.mipLevels is more than 1 then desc.width must be >= pow(2, desc.mipLevels).");
                return result::ErrorInvalidUsage;
            }

            // desc.format is a valid value
            if (desc.format > format::MaxEnum)
            {
                detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.format was not a valid enum value.");
                return result::ErrorInvalidUsage;
            }

            if (desc.format == format::Undefined)
            {
                detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.format was format::Undefined.");
                return result::ErrorInvalidUsage;
            }
            const format_properties props = m_adapter->queryFormatProperties(desc.format);

            if (!props.supported)
            {
                detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.format must be supported. Query support through Adapter::queryFormatProperties().");
                return result::ErrorInvalidUsage;
            }

            if (props.sampleCounts.at(desc.sampleCount) == false)
            {
                detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.format doesn't support desc.sampleCount. Query sample count support per format using Adapter::queryFormatProperties().");
                return result::ErrorInvalidUsage;
            }

            // desc.format against desc.type
            if (props.types.at(desc.type) == false)
            {
                detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.format doesn't support desc.type. Query support for resource_types per format using Adapter::queryFormatProperties().");
                return result::ErrorInvalidUsage;
            }

            // desc.format against desc.usage
            if (props.usage.all(desc.usage) == false)
            {
                detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.format does not support some (or any) of the resource_usage_flag_bits set in desc.usage. desc.format (" + to_string(desc.format) + ") supports the usage flags: " + to_string(props.usage));
                return result::ErrorInvalidUsage;
            }

            // desc.tiling is a valid enum value
            if (desc.tiling > tiling::MaxEnum)
            {
                detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.tiling was not a valid enum value.");
                return result::ErrorInvalidUsage;
            }

            if (desc.tiling == tiling::Linear)
            {
                // desc.tiling against desc.type
                if (desc.type != llri::resource_type::Texture2D)
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.tiling was Linear but desc.type was not Texture2D.");
                    return result::ErrorInvalidUsage;
                }

                // desc.tiling against desc.format
                if (!props.linearTiling)
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.tiling was Linear but desc.format does not support linear tiling.");
                    return result::ErrorInvalidUsage;
                }

                // desc.tiling against depth and samplecount
                if (desc.depthOrArrayLayers != 1)
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.tiling was Linear and desc.type was not Texture3D but desc.depthOrArrayLayers was not 1.");
                    return result::ErrorInvalidUsage;
                }

                // desc.tiling against desc.sampleCount
                if (desc.sampleCount != llri::sample_count::Count1)
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.tiling was Linear but desc.samples was not Count1.");
                    return result::ErrorInvalidUsage;
                }

                // desc.tiling against desc.usage
                llri::resource_usage_flags supportedUsage = llri::resource_usage_flag_bits::TransferSrc | llri::resource_usage_flag_bits::TransferDst;
                if ((desc.usage & ~supportedUsage) != llri::resource_usage_flag_bits::None)
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.tiling was Linear but desc.usage had bits set that were not TransferSrc and/or TransferDst.");
                    return result::ErrorInvalidUsage;
                }
            }
        }
#endif

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto result = impl_createResource(desc, resource);
        detail::impl_pollAPIMessages(m_validationCallbackMessenger);
        return result;
#else
        return impl_createResource(desc, resource);
#endif
    }

    inline void Device::destroyResource(Resource* resource)
    {
        if (!resource)
            return;

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        detail::impl_pollAPIMessages(m_validationCallbackMessenger);
#endif

        impl_destroyResource(resource);
    }
}
