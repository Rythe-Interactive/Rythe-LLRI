/**
 * @file resource.inl
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> //Recursive include technically not necessary but helps with intellisense

namespace LLRI_NAMESPACE
{
    inline std::string to_string(resource_state state)
    {
        switch(state)
        {
            case resource_state::General:
                return "General";
            case resource_state::Upload:
                return "Upload";
            case resource_state::ColorAttachment:
                return "ColorAttachment";
            case resource_state::DepthStencilAttachment:
                return "DepthStencilAttachment";
            case resource_state::DepthStencilAttachmentReadOnly:
                return "DepthStencilAttachmentReadOnly";
            case resource_state::ShaderReadOnly:
                return "ShaderReadOnly";
            case resource_state::ShaderReadWrite:
                return "ShaderReadWrite";
            case resource_state::TransferSrc:
                return "TransferSrc";
            case resource_state::TransferDst:
                return "TransferDst";
            case resource_state::VertexBuffer:
                return "VertexBuffer";
            case resource_state::IndexBuffer:
                return "IndexBuffer";
            case resource_state::ConstantBuffer:
                return "ConstantBuffer";
        }

        return "Invalid resource_state value";
    }

    inline std::string to_string(resource_type type)
    {
        switch(type)
        {
            case resource_type::MemoryOnly:
                return "MemoryOnly";
            case resource_type::Buffer:
                return "Buffer";
            case resource_type::Texture1D:
                return "Texture1D";
            case resource_type::Texture2D:
                return "Texture2D";
            case resource_type::Texture3D:
                return "Texture3D";
        }

        return "Invalid resource_type value";
    }

    inline std::string to_string(texture_format format)
    {
        switch (format)
        {
            case texture_format::Undefined:
                return "Undefined";
            case texture_format::R8UNorm:
                return "R8UNorm";
            case texture_format::R8Norm:
                return "R8Norm";
            case texture_format::R8UInt:
                return "R8UInt";
            case texture_format::R8Int:
                return "R8Int";
            case texture_format::RG8UNorm:
                return "RG8UNorm";
            case texture_format::RG8Norm:
                return "RG8Norm";
            case texture_format::RG8UInt:
                return "RG8UInt";
            case texture_format::RG8Int:
                return "RG8Int";
            case texture_format::RGBA8UNorm:
                return "RGBA8Unorm";
            case texture_format::RGBA8Norm:
                return "RGBA8Norm";
            case texture_format::RGBA8UInt:
                return "RGBA8UInt";
            case texture_format::RGBA8Int:
                return "RGBA8Int";
            case texture_format::RGBA8sRGB:
                return "RGBA8sRGB";
            case texture_format::BGRA8UNorm:
                return "BGRA8Unorm";
            case texture_format::BGRA8sRGB:
                return "BGRA8sRGB";
            case texture_format::RGB10A2UNorm:
                return "RGB10A2UNorm";
            case texture_format::RGB10A2UInt:
                return "RGB10A2UInt";
            case texture_format::R16UNorm:
                return "R16UNorm";
            case texture_format::R16Norm:
                return "R16Norm";
            case texture_format::R16UInt:
                return "R16UInt";
            case texture_format::R16Int:
                return "R16Int";
            case texture_format::R16Float:
                return "R16Float";
            case texture_format::RG16UNorm:
                return "RG16UNorm";
            case texture_format::RG16Norm:
                return "RG16Norm";
            case texture_format::RG16UInt:
                return "RG16UInt";
            case texture_format::RG16Int:
                return "RG16Int";
            case texture_format::RG16Float:
                return "RG16Float";
            case texture_format::RGBA16UNorm:
                return "RGBA16UNorm";
            case texture_format::RGBA16Norm:
                return "RGBA16Norm";
            case texture_format::RGBA16UInt:
                return "RGBA16UInt";
            case texture_format::RGBA16Int:
                return "RGBA16Int";
            case texture_format::RGBA16Float:
                return "RGBA16Float";
            case texture_format::R32UInt:
                return "R32UInt";
            case texture_format::R32Int:
                return "R32Int";
            case texture_format::R32Float:
                return "R32Float";
            case texture_format::RG32UInt:
                return "RG32UInt";
            case texture_format::RG32Int:
                return "RG32Int";
            case texture_format::RG32Float:
                return "RG32Float";
            case texture_format::RGB32UInt:
                return "RGB32UInt";
            case texture_format::RGB32Int:
                return "RGB32Int";
            case texture_format::RGB32Float:
                return "RGB32Float";
            case texture_format::RGBA32UInt:
                return "RGBA32UInt";
            case texture_format::RGBA32Int:
                return "RGBA32Int";
            case texture_format::RGBA32Float:
                return "RGBA32Float";
            case texture_format::D16UNorm:
                return "D16UNorm";
            case texture_format::D24UNormS8UInt:
                return "D24UNormS8UInt";
            case texture_format::D32Float:
                return "D32Float";
            case texture_format::D32FloatS8X24UInt:
                return "D32FloatS8X24UInt";
        }

        return "Invalid texture_format value";
    }

    inline std::string to_string(texture_sample_count count)
    {
        switch(count)
        {
            case texture_sample_count::Count1:
                return "Count1";
            case texture_sample_count::Count2:
                return "Count2";
            case texture_sample_count::Count4:
                return "Count4";
            case texture_sample_count::Count8:
                return "Count8";
            case texture_sample_count::Count16:
                return "Count16";
            case texture_sample_count::Count32:
                return "Count32";
        }
        return "Invalid texture_sample_count value";
    }

    inline std::string to_string(resource_usage_flag_bits bits)
    {
        switch(bits)
        {
            case resource_usage_flag_bits::TransferSrc:
                return "TransferSrc";
            case resource_usage_flag_bits::TransferDst:
                return "TransferDst";
            case resource_usage_flag_bits::Sampled:
                return "Sampled";
            case resource_usage_flag_bits::ShaderWrite:
                return "ShaderWrite";
            case resource_usage_flag_bits::ColorAttachment:
                return "ColorAttachment";
            case resource_usage_flag_bits::DepthStencilAttachment:
                return "DepthStencilAttachment";
            case resource_usage_flag_bits::DenyShaderResource:
                return "DenyShaderResource";
        }

        return "Invalid resource_usage_flag_bits value";
    }

    inline std::string to_string(resource_usage_flags flags)
    {
        std::string out;

        std::unordered_set<resource_usage_flag_bits> allBits = {
            resource_usage_flag_bits::TransferSrc,
            resource_usage_flag_bits::TransferDst,
            resource_usage_flag_bits::Sampled,
            resource_usage_flag_bits::ShaderWrite,
            resource_usage_flag_bits::ColorAttachment,
            resource_usage_flag_bits::DepthStencilAttachment,
            resource_usage_flag_bits::DenyShaderResource
        };

        for (auto elem : allBits)
        {
            if (flags.contains(elem))
            {
                out += " | " + to_string(elem);
                flags.remove(elem);
            }
        }

        // all flags should've been covered and removed
        if (flags != resource_usage_flag_bits::None)
            return "Invalid resource_usage_flags value";

        // remove excessive initial " | "
        if (!out.empty() && out[0] == ' ' && out[1] == '|' && out[2] == ' ')
            out = out.substr(3);

        return out;
    }

    inline std::string to_string(resource_memory_type type)
    {
        switch(type)
        {
            case resource_memory_type::Local:
                return "Local";
            case resource_memory_type::Upload:
                return "Upload";
            case resource_memory_type::Read:
                return "Read";
        }

        return "Invalid resource_memory_type value";
    }

    constexpr resource_desc resource_desc::buffer(resource_usage_flags usage, resource_memory_type memoryType, resource_state initialState, uint32_t sizeInBytes) noexcept
    {
        return {
            resource_type::Buffer,
            usage, memoryType, initialState,
            sizeInBytes, // width = size
            1, 1, 1, // texture sizes defaulted to 1
            texture_sample_count::Count1, texture_format::Undefined, texture_tiling::Optimal, // these parameters are ignored but we set them to reasonable defaults
        };
    }
}
