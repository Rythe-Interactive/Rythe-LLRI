/**
 * @file resource.inl
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
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

    inline std::string to_string(format f)
    {
        switch (f)
        {
            case format::Undefined:
                return "Undefined";
            case format::R8UNorm:
                return "R8UNorm";
            case format::R8Norm:
                return "R8Norm";
            case format::R8UInt:
                return "R8UInt";
            case format::R8Int:
                return "R8Int";
            case format::RG8UNorm:
                return "RG8UNorm";
            case format::RG8Norm:
                return "RG8Norm";
            case format::RG8UInt:
                return "RG8UInt";
            case format::RG8Int:
                return "RG8Int";
            case format::RGBA8UNorm:
                return "RGBA8Unorm";
            case format::RGBA8Norm:
                return "RGBA8Norm";
            case format::RGBA8UInt:
                return "RGBA8UInt";
            case format::RGBA8Int:
                return "RGBA8Int";
            case format::RGBA8sRGB:
                return "RGBA8sRGB";
            case format::BGRA8UNorm:
                return "BGRA8Unorm";
            case format::BGRA8sRGB:
                return "BGRA8sRGB";
            case format::RGB10A2UNorm:
                return "RGB10A2UNorm";
            case format::RGB10A2UInt:
                return "RGB10A2UInt";
            case format::R16UNorm:
                return "R16UNorm";
            case format::R16Norm:
                return "R16Norm";
            case format::R16UInt:
                return "R16UInt";
            case format::R16Int:
                return "R16Int";
            case format::R16Float:
                return "R16Float";
            case format::RG16UNorm:
                return "RG16UNorm";
            case format::RG16Norm:
                return "RG16Norm";
            case format::RG16UInt:
                return "RG16UInt";
            case format::RG16Int:
                return "RG16Int";
            case format::RG16Float:
                return "RG16Float";
            case format::RGBA16UNorm:
                return "RGBA16UNorm";
            case format::RGBA16Norm:
                return "RGBA16Norm";
            case format::RGBA16UInt:
                return "RGBA16UInt";
            case format::RGBA16Int:
                return "RGBA16Int";
            case format::RGBA16Float:
                return "RGBA16Float";
            case format::R32UInt:
                return "R32UInt";
            case format::R32Int:
                return "R32Int";
            case format::R32Float:
                return "R32Float";
            case format::RG32UInt:
                return "RG32UInt";
            case format::RG32Int:
                return "RG32Int";
            case format::RG32Float:
                return "RG32Float";
            case format::RGB32UInt:
                return "RGB32UInt";
            case format::RGB32Int:
                return "RGB32Int";
            case format::RGB32Float:
                return "RGB32Float";
            case format::RGBA32UInt:
                return "RGBA32UInt";
            case format::RGBA32Int:
                return "RGBA32Int";
            case format::RGBA32Float:
                return "RGBA32Float";
            case format::D16UNorm:
                return "D16UNorm";
            case format::D24UNormS8UInt:
                return "D24UNormS8UInt";
            case format::D32Float:
                return "D32Float";
            case format::D32FloatS8X24UInt:
                return "D32FloatS8X24UInt";
        }

        return "Invalid format value";
    }

    inline std::string to_string(sample_count count)
    {
        switch(count)
        {
            case sample_count::Count1:
                return "Count1";
            case sample_count::Count2:
                return "Count2";
            case sample_count::Count4:
                return "Count4";
            case sample_count::Count8:
                return "Count8";
            case sample_count::Count16:
                return "Count16";
            case sample_count::Count32:
                return "Count32";
        }
        return "Invalid sample_count value";
    }

    inline std::string to_string(resource_usage_flag_bits bits)
    {
        switch(bits)
        {
            case resource_usage_flag_bits::None:
                return "None";
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
            case resource_usage_flag_bits::All:
                return to_string(static_cast<resource_usage_flags>(bits));
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

    inline std::string to_string(memory_type type)
    {
        switch(type)
        {
            case memory_type::Local:
                return "Local";
            case memory_type::Upload:
                return "Upload";
            case memory_type::Read:
                return "Read";
        }

        return "Invalid memory_type value";
    }

    inline resource_desc Resource::getDesc() const
    {
        return m_desc;
    }

    constexpr resource_desc resource_desc::buffer(resource_usage_flags usage, memory_type memoryType, resource_state initialState, uint32_t sizeInBytes, uint32_t createNodeMask, uint32_t visibleNodeMask) noexcept
    {
        return {
            createNodeMask, visibleNodeMask,
            resource_type::Buffer,
            usage, memoryType, initialState,
            sizeInBytes, // width = size
            1, 1, 1, // texture sizes defaulted to 1
            sample_count::Count1, format::Undefined // these parameters are ignored but we set them to reasonable defaults
        };
    }
}
