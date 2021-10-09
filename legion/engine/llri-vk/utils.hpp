/**
 * @file utils.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp>
#include <graphics/vulkan/volk.h>
#include <unordered_map>

namespace LLRI_NAMESPACE
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
            }

            throw;
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
            }

            throw;
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
            }

            throw;
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
            }

            throw;
        }

        constexpr VkImageTiling mapTextureTiling(tiling tiling)
        {
            switch (tiling)
            {
                case tiling::Optimal:
                    return VK_IMAGE_TILING_OPTIMAL;
                case tiling::Linear:
                    return VK_IMAGE_TILING_LINEAR;
            }

            throw;
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

        uint32_t findMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t requiredMemoryBits, VkMemoryPropertyFlags requiredFlags);

        unsigned long long nameHash(std::string name);

        /**
         * @brief Finds LLRI standard queue families (Graphics, Compute, Transfer)
        */
        std::map<queue_type, uint32_t> findQueueFamilies(VkPhysicalDevice physicalDevice);
    }
}
