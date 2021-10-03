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
        // In resource creation, there are a lot of combinations that can be incorrect
        // checks for these combinations can get confusing,
        // our rule will be to only check against previously checked variables. e.g. if we check desc.type first, we only need to check that its valid,
        // but if we check desc.usage we must check if its valid with desc.type

        //determines if the node mask is not a power of two -> if it isn't then multiple bits are set
        if ((desc.createNodeMask & (desc.createNodeMask - 1)) != 0)
        {
            detail::apiError("Device::createResource()", result::ErrorInvalidNodeMask, "desc.createNodeMask " + std::to_string(desc.createNodeMask) + " has multiple bits set.");
            return result::ErrorInvalidNodeMask;
        }

        if (desc.createNodeMask >= (1 << m_adapter->queryNodeCount()))
        {
            detail::apiError("Device::createResource()", result::ErrorInvalidNodeMask, "desc.createNodeMask " + std::to_string(desc.createNodeMask) + " has a bit set that is >= (1 << Adapter::queryNodeCount()).");
            return result::ErrorInvalidNodeMask;
        }

        if ((desc.visibleNodeMask & desc.createNodeMask) != desc.createNodeMask)
        {
            detail::apiError("Device::createResource()", result::ErrorInvalidNodeMask, "desc.visibleNodeMask doesn't have at least the same bit set as desc.createNodeMask.");
            return result::ErrorInvalidNodeMask;
        }

        const uint32_t maxVisibleNodeMask = (1 << m_adapter->queryNodeCount()) - 1;
        if (desc.visibleNodeMask > maxVisibleNodeMask)
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
            if (desc.usage.contains(resource_usage_flag_bits::DepthStencilAttachment))
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
            case resource_type::MemoryOnly:
            {
                constexpr resource_usage_flags validUsage = resource_usage_flag_bits::TransferSrc | resource_usage_flag_bits::TransferDst;
                if (!validUsage.contains(desc.usage.value))
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.type is MemoryOnly but desc.usage has invalid resource_usage_flag_bits set. Valid flag bits for this type are: " + to_string(validUsage));
                    return result::ErrorInvalidUsage;
                }
                break;
            }
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
        if (desc.memoryType > resource_memory_type::MaxEnum)
        {
            detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.memoryType (" + std::to_string(static_cast<uint8_t>(desc.memoryType)) + " is not a valid resource_memory_type value.");
            return result::ErrorInvalidUsage;
        }

        // desc.memoryType against desc.type
        if (desc.type == resource_type::MemoryOnly && desc.memoryType != resource_memory_type::Upload)
        {
            detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.type is resource_type::MemoryOnly but desc.memoryType (" + to_string(desc.memoryType) + ") is not resource_memory_type::Upload.");
            return result::ErrorInvalidUsage;
        }

        // desc.memoryType against desc.usage
        switch(desc.memoryType)
        {
            case resource_memory_type::Local:
                // local currently supports all flags
                break;
            case resource_memory_type::Upload:
            {
                if (desc.usage.contains(resource_usage_flag_bits::ShaderWrite))
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.memoryType is resource_memory_type::Upload but desc.usage has the resource_usage_flag_bits::ShaderWrite bit set. Upload resources can not be used as ShaderWrite resources.");
                    return result::ErrorInvalidUsage;
                }
                break;
            }
            case resource_memory_type::Read:
            {
                //TODO: Validate resource_usage_flags support with resource_memory_type::Read
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
            case resource_type::MemoryOnly:
            {
                if (desc.initialState != resource_state::Upload)
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.type was set to MemoryOnly but desc.initialState wasn't one of the following states: resource_state::Upload");
                    return result::ErrorInvalidUsage;
                }
                break;
            }
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
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.initialState was set to Upload but is incompatible with resource_usage_flag_bits::ShaderWrite.");
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
                if (!desc.usage.contains(resource_usage_flag_bits::TransferSrc))
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
            case resource_memory_type::Local:
            {
                if (desc.initialState == resource_state::Upload)
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.memoryType was set to Local but desc.initialState was set to Upload.");
                    return result::ErrorInvalidUsage;
                }
                break;
            }
            case resource_memory_type::Upload:
            {
                if (desc.initialState == resource_state::ShaderReadWrite)
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.memoryType was set to Upload but desc.initialState was set to ShaderReadWrite.");
                    return result::ErrorInvalidUsage;
                }
                break;
            }
            case resource_memory_type::Read:
            {
                //TODO: Validate initialState against resource_memory_type::Read
                break;
            }
        }

        // desc.width is a valid value
        if (desc.width == 0)
        {
            detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "");
        }

        // width, height, depth against desc.type
        switch(desc.type)
        {
            case resource_type::MemoryOnly:
            case resource_type::Buffer:
            {
                if (desc.height > 1)
                    detail::apiWarning("Device::createResource()", "desc.type was resource_type::Buffer or resource_type::MemoryOnly but desc.height was more than 1. This value will be ignored internally but this may be a user error. The size of the created resource will only be determined by desc.width.");

                if (desc.depthOrArrayLayers > 1)
                    detail::apiWarning("Device::createResource()", "desc.type was resource_type::Buffer or resource_type::MemoryOnly but desc.depthOrArrayLayers was more than 1. This value will be ignored internally but this may be a user error. The size of the created resource will only be determined by desc.width.");
                break;
            }
            case resource_type::Texture1D:
            {
                if (desc.height != 1)
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.type was resource_type::Texture1D but desc.height was not 1.");
                break;
            }
            case resource_type::Texture2D:
            {
                if (desc.height < 1)
                {
                    detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.type was resource_type::Texture2D but desc.height was not at least 1.");
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
            if (desc.mipLevels < 1)
            {
                detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.mipLevels was not at least 1.");
                return result::ErrorInvalidUsage;
            }

            // desc.sampleCount is a valid value
            if (desc.sampleCount > texture_sample_count::MaxEnum)
            {
                detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.sampleCount was not a valid enum value.");
                return result::ErrorInvalidUsage;
            }

            // desc.sampleCount against desc.type
            // no current checks

            // desc.sampleCount against desc.usage
            if (desc.usage.contains(resource_usage_flag_bits::ShaderWrite) && desc.sampleCount > texture_sample_count::Count1)
            {
                detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.usage had the ShaderWrite bit set but desc.sampleCount was not Count1.");
                return result::ErrorInvalidUsage;
            }

            // desc.format is a valid value
            if (desc.format > texture_format::MaxEnum)
            {
                detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.format was not a valid enum value.");
                return result::ErrorInvalidUsage;
            }

            // desc.format against desc.type
            // no current checks

            // desc.format against desc.usage
            // TODO desc.format validity against desc.usage
            if (desc.usage.contains(resource_usage_flag_bits::Sampled))
            {
                std::set<texture_format> supportedFormats;
            }
            if (desc.usage.contains(resource_usage_flag_bits::ShaderWrite))
            {
                std::set<texture_format> supportedFormats;
            }
            if (desc.usage.contains(resource_usage_flag_bits::ColorAttachment))
            {
                std::set<texture_format> supportedFormats;
            }
            if (desc.usage.contains(resource_usage_flag_bits::DepthStencilAttachment))
            {
                std::set<texture_format> supportedFormats;
            }

            // desc.tiling is a valid enum value
            if (desc.tiling > texture_tiling::MaxEnum)
            {
                detail::apiError("Device::createResource()", result::ErrorInvalidUsage, "desc.tiling was not a valid enum value.");
                return result::ErrorInvalidUsage;
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
