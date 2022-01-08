/**
 * @file device.inl
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    inline device_desc Device::getDesc() const
    {
        return m_desc;
    }

    inline result Device::getQueue(queue_type type, uint8_t index, Queue** queue)
    {
        LLRI_DETAIL_VALIDATION_REQUIRE(queue != nullptr, result::ErrorInvalidUsage)

        *queue = nullptr;

        LLRI_DETAIL_VALIDATION_REQUIRE(type <= queue_type::MaxEnum, result::ErrorInvalidUsage)

        std::vector<Queue*>* queues = nullptr;
        switch(type)
        {
            case queue_type::Graphics:
            {
                queues = &m_graphicsQueues;

                LLRI_DETAIL_VALIDATION_REQUIRE(index < static_cast<uint8_t>(m_graphicsQueues.size()), result::ErrorExceededLimit)
                break;
            }
            case queue_type::Compute:
            {
                queues = &m_computeQueues;

                LLRI_DETAIL_VALIDATION_REQUIRE(index < static_cast<uint8_t>(m_computeQueues.size()), result::ErrorExceededLimit)
                break;
            }
            case queue_type::Transfer:
            {
                queues = &m_transferQueues;

                LLRI_DETAIL_VALIDATION_REQUIRE(index < static_cast<uint8_t>(m_transferQueues.size()), result::ErrorExceededLimit)
                break;
            }
        }

        *queue = queues->at(index);
        return result::Success;
    }

    inline uint8_t Device::queryQueueCount(queue_type type)
    {
        switch (type)
        {
            case queue_type::Graphics:
                return static_cast<uint8_t>(m_graphicsQueues.size());
            case queue_type::Compute:
                return static_cast<uint8_t>(m_computeQueues.size());
            case queue_type::Transfer:
                return static_cast<uint8_t>(m_transferQueues.size());
        }

        return 0;
    }

    inline result Device::createCommandGroup(const command_group_desc& desc, CommandGroup** cmdGroup)
    {
        LLRI_DETAIL_VALIDATION_REQUIRE(cmdGroup != nullptr, result::ErrorInvalidUsage)

        *cmdGroup = nullptr;

        LLRI_DETAIL_VALIDATION_REQUIRE(desc.type <= queue_type::MaxEnum, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE(queryQueueCount(desc.type) > 0, result::ErrorInvalidUsage)

        LLRI_DETAIL_CALL_IMPL(impl_createCommandGroup(desc, cmdGroup), m_validationCallbackMessenger)
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
        LLRI_DETAIL_POLL_API_MESSAGES(m_validationCallbackMessenger)
    }

    inline result Device::createFence(fence_flags flags, Fence** fence)
    {
        LLRI_DETAIL_VALIDATION_REQUIRE(fence != nullptr, result::ErrorInvalidUsage)

        *fence = nullptr;

        LLRI_DETAIL_VALIDATION_REQUIRE(flags == fence_flag_bits::None || flags == fence_flag_bits::Signaled, result::ErrorInvalidUsage)

        LLRI_DETAIL_CALL_IMPL(impl_createFence(flags, fence), m_validationCallbackMessenger)
    }

    inline void Device::destroyFence(Fence* fence)
    {
        if (!fence)
            return;

        impl_destroyFence(fence);
        LLRI_DETAIL_POLL_API_MESSAGES(m_validationCallbackMessenger)
    }

    inline result Device::waitFences(uint32_t numFences, Fence** fences, uint64_t timeout)
    {
        LLRI_DETAIL_VALIDATION_REQUIRE(fences != nullptr, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE(numFences > 0, result::ErrorInvalidUsage)

#ifdef LLRI_DETAIL_ENABLE_VALIDATION
        for (size_t i = 0; i < numFences; i++)
        {
            LLRI_DETAIL_VALIDATION_REQUIRE_ITER(fences[i] != nullptr, i, result::ErrorInvalidUsage)
            LLRI_DETAIL_VALIDATION_REQUIRE_ITER(fences[i]->m_signaled, i, result::ErrorNotSignaled)
        }
#endif

        LLRI_DETAIL_CALL_IMPL(impl_waitFences(numFences, fences, timeout), m_validationCallbackMessenger)
    }

    inline result Device::waitFence(Fence* fence, uint64_t timeout)
    {
        return waitFences(1, &fence, timeout);
    }

    inline result Device::createSemaphore(Semaphore** semaphore)
    {
        LLRI_DETAIL_VALIDATION_REQUIRE(semaphore != nullptr, result::ErrorInvalidUsage)

        *semaphore = nullptr;

        LLRI_DETAIL_CALL_IMPL(impl_createSemaphore(semaphore), m_validationCallbackMessenger)
    }

    inline void Device::destroySemaphore(Semaphore* semaphore)
    {
        if (!semaphore)
            return;

        impl_destroySemaphore(semaphore);
        LLRI_DETAIL_POLL_API_MESSAGES(m_validationCallbackMessenger)
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
        LLRI_DETAIL_VALIDATION_REQUIRE(resource != nullptr, result::ErrorInvalidUsage)

        *resource = nullptr;

#ifdef LLRI_DETAIL_ENABLE_VALIDATION
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

        // desc.create/visibleNodeMask
        LLRI_DETAIL_VALIDATION_REQUIRE(detail::hasSingleBit(createNodeMask), result::ErrorInvalidNodeMask)
        LLRI_DETAIL_VALIDATION_REQUIRE(createNodeMask < (1 << m_adapter->queryNodeCount()), result::ErrorInvalidNodeMask)
        LLRI_DETAIL_VALIDATION_REQUIRE(visibleNodeMask < (1 << m_adapter->queryNodeCount()), result::ErrorInvalidNodeMask)

        LLRI_DETAIL_VALIDATION_REQUIRE((visibleNodeMask & createNodeMask) == createNodeMask, result::ErrorInvalidNodeMask)

        // desc.type
        LLRI_DETAIL_VALIDATION_REQUIRE(desc.type <= resource_type::MaxEnum, result::ErrorInvalidNodeMask)

        bool isTexture = desc.type == resource_type::Texture1D || desc.type == resource_type::Texture2D || desc.type == resource_type::Texture3D;

        // desc.usage
        LLRI_DETAIL_VALIDATION_REQUIRE(desc.usage <= (resource_usage_flag_bits::TransferSrc |
            resource_usage_flag_bits::TransferDst |
            resource_usage_flag_bits::Sampled |
            resource_usage_flag_bits::ShaderWrite |
            resource_usage_flag_bits::ColorAttachment |
            resource_usage_flag_bits::DepthStencilAttachment |
            resource_usage_flag_bits::DenyShaderResource), result::ErrorInvalidUsage)

        LLRI_DETAIL_VALIDATION_REQUIRE_IF(desc.usage.contains(resource_usage_flag_bits::DenyShaderResource), desc.usage.contains(resource_usage_flag_bits::DepthStencilAttachment), result::ErrorInvalidUsage)

        if (desc.usage.contains(resource_usage_flag_bits::DenyShaderResource))
        {
            constexpr resource_usage_flags validUsage = resource_usage_flag_bits::DenyShaderResource | resource_usage_flag_bits::DepthStencilAttachment |
                resource_usage_flag_bits::TransferSrc | resource_usage_flag_bits::TransferDst;

            LLRI_DETAIL_VALIDATION_REQUIRE_MESSAGE(
                validUsage.contains(desc.usage.value),
                "desc.usage ( " + to_string(desc.usage) + ") has the DenyShaderResource bit set but it has shader related usage flags set. Allowed flags are: " + to_string(validUsage),
                result::ErrorInvalidUsage)
        }

        // desc.usage against desc.type
        switch(desc.type)
        {
            case resource_type::Buffer:
            {
                constexpr resource_usage_flags validUsage = resource_usage_flag_bits::TransferSrc | resource_usage_flag_bits::TransferDst | resource_usage_flag_bits::ShaderWrite;
                LLRI_DETAIL_VALIDATION_REQUIRE_MESSAGE(
                    validUsage.contains(desc.usage.value),
                    "desc.type is Buffer but desc.usage has invalid resource_usage_flag_bits set. Valid flag bits for this type are: " + to_string(validUsage),
                    result::ErrorInvalidUsage)
                break;
            }
            case resource_type::Texture1D:
            case resource_type::Texture2D:
            case resource_type::Texture3D:
                break; // textures currently support all resource usage flags
        }

        // desc.memoryType
        LLRI_DETAIL_VALIDATION_REQUIRE(desc.memoryType <= memory_type::MaxEnum, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE((desc.type == resource_type::Buffer) || (desc.type != resource_type::Buffer && desc.memoryType == memory_type::Local), result::ErrorInvalidUsage)

        LLRI_DETAIL_VALIDATION_REQUIRE_IF(desc.memoryType == memory_type::Upload, desc.usage.none(resource_usage_flag_bits::ShaderWrite | resource_usage_flag_bits::ColorAttachment | resource_usage_flag_bits::DepthStencilAttachment | resource_usage_flag_bits::DenyShaderResource), result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(desc.memoryType == memory_type::Read, desc.usage.none( resource_usage_flag_bits::ShaderWrite | resource_usage_flag_bits::ColorAttachment | resource_usage_flag_bits::DepthStencilAttachment | resource_usage_flag_bits::DenyShaderResource), result::ErrorInvalidUsage)

        // desc.initialState is a valid value
        LLRI_DETAIL_VALIDATION_REQUIRE(desc.initialState <= resource_state::MaxEnum, result::ErrorInvalidUsage)

        switch(desc.type)
        {
            case resource_type::Buffer:
            {
                const std::unordered_set<resource_state> validStates { resource_state::General, resource_state::Upload, resource_state::ShaderReadOnly, resource_state::ShaderReadWrite, resource_state::TransferSrc, resource_state::TransferDst, resource_state::VertexBuffer, resource_state::IndexBuffer, resource_state::ConstantBuffer };

                LLRI_DETAIL_VALIDATION_REQUIRE_MESSAGE(
                    validStates.find(desc.initialState) != validStates.end(),
                    "desc.type was set to Buffer but desc.initialState wasn't one of the following states: General, Upload, ShaderReadOnly, ShaderReadWrite, TransferSrc, TransferDst, VertexBuffer, IndexBuffer, ConstantBuffer.",
                    result::ErrorInvalidUsage)
                break;
            }
            case resource_type::Texture1D:
            case resource_type::Texture2D:
            case resource_type::Texture3D:
            {
                const std::unordered_set<resource_state> validStates { resource_state::General, resource_state::Upload, resource_state::ColorAttachment, resource_state::DepthStencilAttachment, resource_state::DepthStencilAttachmentReadOnly, resource_state::ShaderReadOnly, resource_state::ShaderReadWrite, resource_state::TransferSrc, resource_state::TransferDst };

                LLRI_DETAIL_VALIDATION_REQUIRE_MESSAGE(
                    validStates.find(desc.initialState) != validStates.end(),
                    "desc.type was set to Texture1D, Texture2D or Texture3D but desc.initialState wasn't one of the following states: General, Upload, ColorAttachment, DepthStencilAttachmentReadWrite, DepthStencilAttachmentReadOnly, ShaderReadOnly, ShaderReadWrite, TransferSrc, TransferDst.",
                    result::ErrorInvalidUsage)
                break;
            }
        }

        LLRI_DETAIL_VALIDATION_REQUIRE_IF(desc.initialState == resource_state::Upload, desc.usage.contains(resource_usage_flag_bits::ShaderWrite), result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(desc.initialState == resource_state::ColorAttachment, desc.usage.contains(resource_usage_flag_bits::ColorAttachment), result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(desc.initialState == resource_state::DepthStencilAttachment, desc.usage.contains(resource_usage_flag_bits::DepthStencilAttachment), result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(desc.initialState == resource_state::DepthStencilAttachmentReadOnly, desc.usage.contains(resource_usage_flag_bits::DepthStencilAttachment), result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(desc.initialState == resource_state::ShaderReadWrite, desc.usage.contains(resource_usage_flag_bits::ShaderWrite), result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(desc.initialState == resource_state::TransferSrc, desc.usage.contains(resource_usage_flag_bits::TransferSrc), result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(desc.initialState == resource_state::TransferDst, desc.usage.contains(resource_usage_flag_bits::TransferDst), result::ErrorInvalidUsage)

        LLRI_DETAIL_VALIDATION_REQUIRE_IF(desc.memoryType == memory_type::Local, desc.initialState != resource_state::Upload, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(desc.memoryType == memory_type::Upload, desc.initialState != resource_state::Upload, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(desc.memoryType == memory_type::Read, desc.initialState != resource_state::TransferDst, result::ErrorInvalidUsage)

        LLRI_DETAIL_VALIDATION_REQUIRE(desc.width > 0, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE(desc.width <= 16348, result::ErrorInvalidUsage)

        LLRI_DETAIL_VALIDATION_REQUIRE_IF(isTexture, desc.height > 0, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(isTexture, desc.height <= 16348, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(isTexture, desc.depthOrArrayLayers > 0, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(isTexture, desc.depthOrArrayLayers <= 16348, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(isTexture, desc.mipLevels > 0, result::ErrorInvalidUsage)

        LLRI_DETAIL_VALIDATION_REQUIRE_IF(desc.type == resource_type::Texture1D, desc.height == 1, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(desc.type == resource_type::Texture2D, desc.height > 0, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(desc.type == resource_type::Texture3D, desc.height > 0, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(desc.type == resource_type::Texture3D, desc.height <= 2048, result::ErrorInvalidUsage)

        LLRI_DETAIL_VALIDATION_REQUIRE_IF(isTexture && desc.width == 1, desc.mipLevels == 1, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(isTexture, desc.sampleCount <= sample_count::MaxEnum, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(desc.type != resource_type::Texture2D, desc.sampleCount == sample_count::Count1, result::ErrorInvalidUsage)
        
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(isTexture && desc.usage.contains(resource_usage_flag_bits::ShaderWrite), desc.sampleCount != sample_count::Count1, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(isTexture && desc.sampleCount > sample_count::Count1, desc.usage.contains(resource_usage_flag_bits::ColorAttachment) || desc.usage.contains(resource_usage_flag_bits::DepthStencilAttachment), result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(isTexture && desc.mipLevels > 1, desc.sampleCount == sample_count::Count1, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(isTexture && desc.mipLevels > 1, desc.width >= std::pow(2, desc.mipLevels), result::ErrorInvalidUsage)

        LLRI_DETAIL_VALIDATION_REQUIRE_IF(isTexture, desc.textureFormat <= format::MaxEnum, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(isTexture, desc.textureFormat != format::Undefined, result::ErrorInvalidUsage)

        const format_properties formatProperties = m_adapter->queryFormatProperties(desc.textureFormat);

        LLRI_DETAIL_VALIDATION_REQUIRE_IF(isTexture, formatProperties.supported, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(isTexture, formatProperties.sampleCounts.at(desc.sampleCount) != false, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(isTexture, formatProperties.types.at(desc.type) != false, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE_IF(isTexture, formatProperties.usage.all(desc.usage), result::ErrorInvalidUsage)
#endif

        LLRI_DETAIL_CALL_IMPL(impl_createResource(desc, resource), m_validationCallbackMessenger)
    }

    inline void Device::destroyResource(Resource* resource)
    {
        if (!resource)
            return;

        impl_destroyResource(resource);
        LLRI_DETAIL_POLL_API_MESSAGES(m_validationCallbackMessenger)
    }
}
