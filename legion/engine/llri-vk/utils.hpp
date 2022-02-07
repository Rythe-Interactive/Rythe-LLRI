/**
 * @file utils.hpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <llri/llri.hpp>

#if defined(__ANDROID__)
    #define VK_USE_PLATFORM_ANDROID_KHR
#elif defined (__APPLE__)
    #define VK_USE_PLATFORM_METAL_EXT
#elif defined(__linux__)
    #define VK_USE_PLATFORM_XCB_KHR
    #define VK_USE_PLATFORM_XLIB_KHR
#elif defined(_WIN32)
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <graphics/vulkan/volk.h>

// Linux X11 defines None which clashes with flags::None
#ifdef __linux__
#undef None
#undef Success
#endif

namespace llri
{
    namespace internal
    {
        static bool isVolkInitialized = false;

        inline void lazyInitializeVolk()
        {
            if (!isVolkInitialized)
            {
                volkInitialize();
                isVolkInitialized = true;
            }
        }
    
#ifdef VK_USE_PLATFORM_METAL_EXT
        void* getCAMetalLayer(void* nsWindow);
#endif

        using layer_map = std::unordered_map<unsigned long long, VkLayerProperties>;
        using extension_map = std::unordered_map<unsigned long long, VkExtensionProperties>;

        /**
         * @brief Helps prevent loading in layers multiple times by loading them in as a static variable in this function
        */
        const layer_map& queryAvailableLayers();
        /**
         * @brief Helper function that maps extensions to their names
        */
        const extension_map& queryAvailableExtensions();

        result mapVkResult(VkResult result);

        constexpr VkCommandBufferLevel mapCommandListUsage(command_list_usage usage)
        {
            switch (usage)
            {
                case command_list_usage::Direct:
                    return VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                case command_list_usage::Indirect:
                    return VK_COMMAND_BUFFER_LEVEL_SECONDARY;
                default:
                    break;
            }

            return {};
        }

        constexpr VkImageType mapTextureType(resource_type type)
        {
            switch (type)
            {
                case resource_type::Buffer: break;
                case resource_type::Texture1D:
                    return VK_IMAGE_TYPE_1D;
                case resource_type::Texture2D:
                    return VK_IMAGE_TYPE_2D;
                case resource_type::Texture3D:
                    return VK_IMAGE_TYPE_3D;
                default:
                    break;
            }

            return {};
        }

        constexpr format mapVkFormat(VkFormat format)
        {
            switch (format)
            {
                case VK_FORMAT_UNDEFINED:
                    return format::Undefined;
                case VK_FORMAT_R8_UNORM:
                    return format::R8UNorm;
                case VK_FORMAT_R8_SNORM:
                    return format::R8Norm;
                case VK_FORMAT_R8_UINT:
                    return format::R8UInt;
                case VK_FORMAT_R8_SINT:
                    return format::R8Int;
                case VK_FORMAT_R8G8_UNORM:
                    return format::RG8UNorm;
                case VK_FORMAT_R8G8_SNORM:
                    return format::RG8Norm;
                case VK_FORMAT_R8G8_UINT:
                    return format::RG8UInt;
                case VK_FORMAT_R8G8_SINT:
                    return format::RG8Int;
                case VK_FORMAT_R8G8B8A8_UNORM:
                    return format::RGBA8UNorm;
                case VK_FORMAT_R8G8B8A8_SNORM:
                    return format::RGBA8Norm;
                case VK_FORMAT_R8G8B8A8_UINT:
                    return format::RGBA8UInt;
                case VK_FORMAT_R8G8B8A8_SINT:
                    return format::RGBA8Int;
                case VK_FORMAT_R8G8B8A8_SRGB:
                    return format::RGBA8sRGB;
                case VK_FORMAT_B8G8R8A8_UNORM:
                    return format::BGRA8UNorm;
                case VK_FORMAT_B8G8R8A8_SRGB:
                    return format::BGRA8sRGB;
                case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
                    return format::RGB10A2UNorm;
                case VK_FORMAT_A2R10G10B10_UINT_PACK32:
                    return format::RGB10A2UInt;
                case VK_FORMAT_R16_UNORM:
                    return format::R16UNorm;
                case VK_FORMAT_R16_SNORM:
                    return format::R16Norm;
                case VK_FORMAT_R16_UINT:
                    return format::R16UInt;
                case VK_FORMAT_R16_SINT:
                    return format::R16Int;
                case VK_FORMAT_R16_SFLOAT:
                    return format::R16Float;
                case VK_FORMAT_R16G16_UNORM:
                    return format::RG16UNorm;
                case VK_FORMAT_R16G16_SNORM:
                    return format::RG16Norm;
                case VK_FORMAT_R16G16_UINT:
                    return format::RG16UInt;
                case VK_FORMAT_R16G16_SINT:
                    return format::RG16Int;
                case VK_FORMAT_R16G16_SFLOAT:
                    return format::RG16Float;
                case VK_FORMAT_R16G16B16A16_UNORM:
                    return format::RGBA16UNorm;
                case VK_FORMAT_R16G16B16A16_SNORM:
                    return format::RGBA16Norm;
                case VK_FORMAT_R16G16B16A16_UINT:
                    return format::RGBA16UInt;
                case VK_FORMAT_R16G16B16A16_SINT:
                    return format::RGBA16Int;
                case VK_FORMAT_R16G16B16A16_SFLOAT:
                    return format::RGBA16Float;
                case VK_FORMAT_R32_UINT:
                    return format::R32UInt;
                case VK_FORMAT_R32_SINT:
                    return format::R32Int;
                case VK_FORMAT_R32_SFLOAT:
                    return format::R32Float;
                case VK_FORMAT_R32G32_UINT:
                    return format::RG32UInt;
                case VK_FORMAT_R32G32_SINT:
                    return format::RG32Int;
                case VK_FORMAT_R32G32_SFLOAT:
                    return format::RG32Float;
                case VK_FORMAT_R32G32B32_UINT:
                    return format::RGB32UInt;
                case VK_FORMAT_R32G32B32_SINT:
                    return format::RGB32Int;
                case VK_FORMAT_R32G32B32_SFLOAT:
                    return format::RGB32Float;
                case VK_FORMAT_R32G32B32A32_UINT:
                    return format::RGBA32UInt;
                case VK_FORMAT_R32G32B32A32_SINT:
                    return format::RGBA32Int;
                case VK_FORMAT_R32G32B32A32_SFLOAT:
                    return format::RGBA32Float;
                case VK_FORMAT_D16_UNORM:
                    return format::D16UNorm;
                case VK_FORMAT_D24_UNORM_S8_UINT:
                    return format::D24UNormS8UInt;
                case VK_FORMAT_D32_SFLOAT:
                    return format::D32Float;
                case VK_FORMAT_D32_SFLOAT_S8_UINT:
                    return format::D32FloatS8X24UInt;
                default:
                    break;
            }

            return format::Undefined;
        }

        constexpr VkFormat mapTextureFormat(format format)
        {
            switch (format)
            {
                case format::Undefined:
                    return VK_FORMAT_UNDEFINED;
                case format::R8UNorm:
                    return VK_FORMAT_R8_UNORM;
                case format::R8Norm:
                    return VK_FORMAT_R8_SNORM;
                case format::R8UInt:
                    return VK_FORMAT_R8_UINT;
                case format::R8Int:
                    return VK_FORMAT_R8_SINT;
                case format::RG8UNorm:
                    return VK_FORMAT_R8G8_UNORM;
                case format::RG8Norm:
                    return VK_FORMAT_R8G8_SNORM;
                case format::RG8UInt:
                    return VK_FORMAT_R8G8_UINT;
                case format::RG8Int:
                    return VK_FORMAT_R8G8_SINT;
                case format::RGBA8UNorm:
                    return VK_FORMAT_R8G8B8A8_UNORM;
                case format::RGBA8Norm:
                    return VK_FORMAT_R8G8B8A8_SNORM;
                case format::RGBA8UInt:
                    return VK_FORMAT_R8G8B8A8_UINT;
                case format::RGBA8Int:
                    return VK_FORMAT_R8G8B8A8_SINT;
                case format::RGBA8sRGB:
                    return VK_FORMAT_R8G8B8A8_SRGB;
                case format::BGRA8UNorm:
                    return VK_FORMAT_B8G8R8A8_UNORM;
                case format::BGRA8sRGB:
                    return VK_FORMAT_B8G8R8A8_SRGB;
                case format::RGB10A2UNorm:
                    return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
                case format::RGB10A2UInt:
                    return VK_FORMAT_A2R10G10B10_UINT_PACK32;
                case format::R16UNorm:
                    return VK_FORMAT_R16_UNORM;
                case format::R16Norm:
                    return VK_FORMAT_R16_SNORM;
                case format::R16UInt:
                    return VK_FORMAT_R16_UINT;
                case format::R16Int:
                    return VK_FORMAT_R16_SINT;
                case format::R16Float:
                    return VK_FORMAT_R16_SFLOAT;
                case format::RG16UNorm:
                    return VK_FORMAT_R16G16_UNORM;
                case format::RG16Norm:
                    return VK_FORMAT_R16G16_SNORM;
                case format::RG16UInt:
                    return VK_FORMAT_R16G16_UINT;
                case format::RG16Int:
                    return VK_FORMAT_R16G16_SINT;
                case format::RG16Float:
                    return VK_FORMAT_R16G16_SFLOAT;
                case format::RGBA16UNorm:
                    return VK_FORMAT_R16G16B16A16_UNORM;
                case format::RGBA16Norm:
                    return VK_FORMAT_R16G16B16A16_SNORM;
                case format::RGBA16UInt:
                    return VK_FORMAT_R16G16B16A16_UINT;
                case format::RGBA16Int:
                    return VK_FORMAT_R16G16B16A16_SINT;
                case format::RGBA16Float:
                    return VK_FORMAT_R16G16B16A16_SFLOAT;
                case format::R32UInt:
                    return VK_FORMAT_R32_UINT;
                case format::R32Int:
                    return VK_FORMAT_R32_SINT;
                case format::R32Float:
                    return VK_FORMAT_R32_SFLOAT;
                case format::RG32UInt:
                    return VK_FORMAT_R32G32_UINT;
                case format::RG32Int:
                    return VK_FORMAT_R32G32_SINT;
                case format::RG32Float:
                    return VK_FORMAT_R32G32_SFLOAT;
                case format::RGB32UInt:
                    return VK_FORMAT_R32G32B32_UINT;
                case format::RGB32Int:
                    return VK_FORMAT_R32G32B32_SINT;
                case format::RGB32Float:
                    return VK_FORMAT_R32G32B32_SFLOAT;
                case format::RGBA32UInt:
                    return VK_FORMAT_R32G32B32A32_UINT;
                case format::RGBA32Int:
                    return VK_FORMAT_R32G32B32A32_SINT;
                case format::RGBA32Float:
                    return VK_FORMAT_R32G32B32A32_SFLOAT;
                case format::D16UNorm:
                    return VK_FORMAT_D16_UNORM;
                case format::D24UNormS8UInt:
                    return VK_FORMAT_D24_UNORM_S8_UINT;
                case format::D32Float:
                    return VK_FORMAT_D32_SFLOAT;
                case format::D32FloatS8X24UInt:
                    return VK_FORMAT_D32_SFLOAT_S8_UINT;
                default:
                    break;
            }

            return VK_FORMAT_UNDEFINED;
        }

        constexpr VkImageLayout mapResourceState(resource_state state)
        {
            switch (state)
            {
                case resource_state::General:
                    return VK_IMAGE_LAYOUT_GENERAL;
                case resource_state::Upload:
                    return VK_IMAGE_LAYOUT_GENERAL;
                case resource_state::ColorAttachment:
                    return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                case resource_state::DepthStencilAttachment:
                    return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                case resource_state::DepthStencilAttachmentReadOnly:
                    return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
                case resource_state::ShaderReadOnly:
                    return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                case resource_state::ShaderReadWrite:
                    return VK_IMAGE_LAYOUT_GENERAL;
                case resource_state::TransferSrc:
                    return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                case resource_state::TransferDst:
                    return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                case resource_state::VertexBuffer:
                    return VK_IMAGE_LAYOUT_GENERAL;
                case resource_state::IndexBuffer:
                    return VK_IMAGE_LAYOUT_GENERAL;
                case resource_state::ConstantBuffer:
                    return VK_IMAGE_LAYOUT_GENERAL;
                default:
                    break;
            }

            return VK_IMAGE_LAYOUT_GENERAL;
        }
    
        constexpr VkAccessFlags mapStateToAccess(resource_state state)
        {
            constexpr std::array<VkAccessFlags, static_cast<size_t>(resource_state::MaxEnum) + 1> map {
                static_cast<VkAccessFlagBits>(0),
                VK_ACCESS_HOST_WRITE_BIT,
                VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
                VK_ACCESS_SHADER_READ_BIT,
                VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
                VK_ACCESS_TRANSFER_READ_BIT,
                VK_ACCESS_TRANSFER_WRITE_BIT,
                VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
                VK_ACCESS_INDEX_READ_BIT,
                VK_ACCESS_UNIFORM_READ_BIT
            };
            
            return map[static_cast<size_t>(state)];
        }
    
        constexpr VkPipelineStageFlags mapStateToPipelineStage(resource_state state)
        {
            constexpr std::array<VkPipelineStageFlags, static_cast<size_t>(resource_state::MaxEnum) + 1> map {
                VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                VK_PIPELINE_STAGE_HOST_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
                VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
                VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
            };
            
            return map[static_cast<size_t>(state)];
        }

        constexpr VkImageUsageFlags mapTextureUsage(resource_usage_flags usage)
        {
            VkImageUsageFlags output = 0;

            if (usage.contains(resource_usage_flag_bits::TransferSrc))
                output |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
            if (usage.contains(resource_usage_flag_bits::TransferDst))
                output |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            if (usage.contains(resource_usage_flag_bits::Sampled))
                output |= VK_IMAGE_USAGE_SAMPLED_BIT;
            if (usage.contains(resource_usage_flag_bits::ShaderWrite))
                output |= VK_IMAGE_USAGE_STORAGE_BIT;
            if (usage.contains(resource_usage_flag_bits::ColorAttachment))
                output |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            if (usage.contains(resource_usage_flag_bits::DepthStencilAttachment))
                output |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

            return output;
        }

        constexpr resource_usage_flags mapVkImageUsage(const VkImageUsageFlags flags)
        {
            resource_usage_flags output;

            if ((flags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) == VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
                output |= resource_usage_flag_bits::TransferSrc;
            if ((flags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) == VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                output |= resource_usage_flag_bits::TransferDst;
            if ((flags & VK_IMAGE_USAGE_SAMPLED_BIT) == VK_IMAGE_USAGE_SAMPLED_BIT)
                output |= resource_usage_flag_bits::Sampled;
            if ((flags & VK_IMAGE_USAGE_STORAGE_BIT) == VK_IMAGE_USAGE_STORAGE_BIT)
                output |= resource_usage_flag_bits::ShaderWrite;
            if ((flags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) == VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
                output |= resource_usage_flag_bits::ColorAttachment;
            if ((flags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
                output |= resource_usage_flag_bits::DepthStencilAttachment;

            return output;
        }

        constexpr VkBufferUsageFlags mapBufferUsage(resource_usage_flags usage)
        {
            VkBufferUsageFlags output = 0;

            if (usage.contains(resource_usage_flag_bits::TransferSrc))
                output |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            if (usage.contains(resource_usage_flag_bits::TransferDst))
                output |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            if (usage.contains(resource_usage_flag_bits::ShaderWrite))
                output |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            return output;
        }

        constexpr VkMemoryPropertyFlags mapMemoryType(memory_type type)
        {
            VkMemoryPropertyFlags memFlags = 0;
            switch (type)
            {
                case memory_type::Local:
                {
                    memFlags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
                    break;
                }
                case memory_type::Upload:
                {
                    memFlags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
                    memFlags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                    break;
                }
                case memory_type::Read:
                {
                    memFlags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
                    memFlags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                    break;
                }
            }
            return memFlags;
        }

        constexpr VkPresentModeKHR mapPresentMode(present_mode_ext presentMode)
        {
            switch(presentMode)
            {
                case present_mode_ext::Immediate:
                    return VK_PRESENT_MODE_IMMEDIATE_KHR;
                case present_mode_ext::Fifo:
                    return VK_PRESENT_MODE_FIFO_KHR;
                case present_mode_ext::Mailbox:
                    return VK_PRESENT_MODE_MAILBOX_KHR;
            }

            throw;
        }

        constexpr present_mode_ext mapVkPresentMode(VkPresentModeKHR presentMode)
        {
            switch(presentMode)
            {
                case VK_PRESENT_MODE_IMMEDIATE_KHR:
                    return present_mode_ext::Immediate;
                case VK_PRESENT_MODE_FIFO_KHR:
                    return present_mode_ext::Fifo;
                case VK_PRESENT_MODE_MAILBOX_KHR:
                    return present_mode_ext::Mailbox;

                case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
                case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
                case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
                case VK_PRESENT_MODE_MAX_ENUM_KHR:
                    break;
                    
                default:
                    break;
            }

            return static_cast<present_mode_ext>(std::numeric_limits<uint8_t>::max());
        }

        uint32_t findMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t requiredMemoryBits, VkMemoryPropertyFlags requiredFlags);

        /**
         * @brief Utility function for hashing strings  in compile time
        */
        template<size_t N>
        constexpr uint64_t nameHash(const char(&name)[N]) noexcept
        {
            uint64_t hash = 0xcbf29ce484222325;
            constexpr uint64_t prime = 0x00000100000001b3;

            size_t size = N;
            if (name[size - 1] == '\0')
                size--;

            for (size_t i = 0; i < size; i++)
            {
                hash = hash ^ static_cast<const uint8_t>(name[i]);
                hash *= prime;
            }

            return hash;
        }
    
        /**
         * @brief Finds LLRI standard queue families (Graphics, Compute, Transfer)
        */
        std::unordered_map<queue_type, uint32_t> findQueueFamilies(VkPhysicalDevice physicalDevice);
    }
}
