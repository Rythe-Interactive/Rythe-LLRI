/**
 * @file resource.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <cstdint>
#include <string>
#include <llri/detail/flags.hpp>

namespace LLRI_NAMESPACE
{
    /**
     * @brief Describes the current state of a Resource. Resources are assigned a state upon creation using the resource_desc::initialState field. Afterwards they can transition to other states using resource barriers.
    */
    enum struct resource_state : uint8_t
    {
        /**
         * @brief All types of device access are allowed in this state, but it is not as optimal as more explicit states.
         *
         * A resource **must** be in this state if it wishes to be used by a Transfer Queue.
        */
        General,
        /**
         * @brief The resource can be mapped and data can be written to it from the CPU.
        */
        Upload,
        /**
         * @brief The texture is used as a color attachment, meaning it can be written to by the fixed graphics pipeline.
         * @note resource_type::Texture1D/2D/3D only.
        */
        ColorAttachment,
        /**
         * @brief The texture is used as a depth stencil attachment, allowing both read and write access from the fixed graphics pipeline.
         * @note resource_type::Texture1D/2D/3D only.
        */
        DepthStencilAttachment,
        /**
         * @brief The texture is used as a depth stencil attachment, but only allows read operations from the fixed graphics pipeline.
        */
        DepthStencilAttachmentReadOnly,
        /**
         * @brief The resource is used as a read-only resource from shaders.
        */
        ShaderReadOnly,
        /**
         * @brief The resource is used as a read-write resource from shaders.
         *
         * Note that if a read-write resource is used in multiple consecutive passes/dispatches without transition then a resource barrier should be used to prevent simultaneous read/writes.
        */
        ShaderReadWrite,
        /**
         * @brief The resource is used as the source of a transfer operation.
        */
        TransferSrc,
        /**
         * @brief The resource is used as the destination of a transfer operation.
        */
        TransferDst,
        /**
         * @brief The resource is used as a vertex buffer.
         * @note resource_type::Buffer only.
        */
        VertexBuffer,
        /**
         * @brief The resource is used as an index buffer.
         * @note resource_type::Buffer only.
        */
        IndexBuffer,
        /**
         * @brief The resource is used as a constant buffer.
         * @note resource_type::Buffer only.
        */
        ConstantBuffer,
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = ConstantBuffer
    };
    
    /**
     * @brief Converts a resource_state to a string.
     * @return The enum value as a string, or "Invalid resource_state value" if the value was not recognized as an enum member.
    */
    std::string to_string(resource_state state);

    /**
     * @brief The type of resource. This determines how the memory for the resource is allocated and how the resource can be used.
    */
    enum struct resource_type : uint8_t
    {
        /**
         * @brief A one-dimensional unformatted chunk of data. Buffers can't be sampled like textures, but they can be read from or written to.
        */
        Buffer,
        /**
         * @brief A one-dimensional texture. 1D textures only have a width, which is equivalent to the number of texels in the 1D texture. The size of the texels may vary per texture, depending on the texture's format.
        */
        Texture1D,
        /**
         * @brief A two-dimensional textures. 2D textures are described by a width and height, their size is equivalent to width * height * texel size. The size of the texels may vary per texture, depending on the texture's format.
        */
        Texture2D,
        /**
         * @brief A three-dimensional texture. 3D textures are described by a width, height, and depth, their size is equivalent to width * height * depth * texel size. The size of the texels may vary per texture, depending on the texture's format.
        */
        Texture3D,
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = Texture3D
    };

    /**
     * @brief Converts a resource_type to a string.
     * @return The enum value as a string, or "Invalid resource_type value" if the value was not recognized as an enum member.
    */
    std::string to_string(resource_type type);

    /**
     * @brief Describes the contents of a single texel in a texture.
     *
     * All enum values follow the same naming format.
     *
     * - Components: The components of the format are described by R, G, B, and A (red, green, blue, alpha respectively). The order of the components in the format describe the order in which they are stored. Some formats contain the D and S components, which stand for Depth and Stencil. If the format has an "X" component, the bits described may be allocated but will be unused.
     * - Size: Components are usually directly followed by their respective size in bits. If consecutive components are the same size, their size can be found after the last consecutive component instead. The size of a texel is the combined size of each component, meaning R8G8B8A8 has 4 8-bit components, meaning the texel is of size 32.
     * - Modifiers: The type of all of the components in the texel is defined at the end of each format name. The names mean the following:
     *  - UNorm: unsigned normalized integer. Interpreted in the resource as an unsigned integer, but in shaders it is interpreted as a floating point value in the range [0, 1].
     *  - Norm: signed normalized integer. Interpreted in the resource as a signed integer, but in shaders it is interpreted as a floating point value in the range [-1, 1].
     *  - UInt: unsigned integer.
     *  - Int: signed integer.
     *  - Float: signed float.
     *  - sRGB: unsigned normalized data with sRGB nonlinear encoding.
    */
    enum struct texture_format : uint8_t
    {
        Undefined,

        R8UNorm,
        R8Norm,
        R8UInt,
        R8Int,

        RG8UNorm,
        RG8Norm,
        RG8UInt,
        RG8Int,

        RGBA8UNorm,
        RGBA8Norm,
        RGBA8UInt,
        RGBA8Int,
        RGBA8sRGB,

        BGRA8UNorm,
        BGRA8sRGB,

        RGB10A2UNorm,
        RGB10A2UInt,

        R16UNorm,
        R16Norm,
        R16UInt,
        R16Int,
        R16Float,

        RG16UNorm,
        RG16Norm,
        RG16UInt,
        RG16Int,
        RG16Float,

        RGBA16UNorm,
        RGBA16Norm,
        RGBA16UInt,
        RGBA16Int,
        RGBA16Float,

        R32UInt,
        R32Int,
        R32Float,

        RG32UInt,
        RG32Int,
        RG32Float,

        RGB32UInt,
        RGB32Int,
        RGB32Float,

        RGBA32UInt,
        RGBA32Int,
        RGBA32Float,

        D16UNorm,
        D24UNormS8UInt,
        D32Float,
        D32FloatS8X24UInt,
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = D32FloatS8X24UInt
    };

    /**
     * @brief Converts a texture_format to a string.
     * @return The enum value as a string, or "Invalid texture_format value" if the value was not recognized as an enum member.
    */
    std::string to_string(texture_format format);

    /**
     * @brief Multi-sampling - the number of samples per pixel.
    */
    enum struct texture_sample_count : uint8_t
    {
        Count1 = 1,
        Count2 = 2,
        Count4 = 4,
        Count8 = 8,
        Count16 = 16,
        Count32 = 32,
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = Count32
    };

    /**
     * @brief Converts a texture_sample_count to a string.
     * @return The enum value as a string, or "Invalid texture_sample_count value" if the value was not recognized as an enum member.
    */
    std::string to_string(texture_sample_count count);

    /**
     * @brief Specify how a texture's data is stored.
    */
    enum struct texture_tiling : uint8_t
    {
        /**
         * @brief Optimal tiling allows implementation-dependent arrangement, which results in the most efficient memory access.
        */
        Optimal,
        /**
         * @brief Texels are laid out linearly, in a row-major order.
        */
        Linear,
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = Linear
    };

    /**
     * @brief Flag bits that describe how the resource will be allowed to be used. Each bit describes an enabled (or explicitly disabled) usage.
    */
    enum struct resource_usage_flag_bits : uint16_t
    {
        /**
         * @brief Default resource usage.
        */
        None = 0,
        /**
         * @brief The resource is allowed to be copied from.
        */
        TransferSrc = 1 << 0,
        /**
         * @brief The resource is allowed to be copied to.
        */
        TransferDst = 1 << 1,
        /**
         * @brief If the resource is a texture, enabling this flag allows the texture to be sampled from.
        */
        Sampled = 1 << 2,
        /**
         * @brief The resource is allowed be written to from within a shader.
        */
        ShaderWrite = 1 << 3,
        /**
         * @brief The resource is allowed to be used as a color attachment.
        */
        ColorAttachment = 1 << 4,
        /**
         * @brief The resource is allowed to be used as a depth stencil attachment
        */
        DepthStencilAttachment = 1 << 5,
        /**
         * @brief The resource is not allowed to be used in a programmable shader.
         *
         * On some implementations, enabling this flag **may** increase bandwidth for depth stencil textures. Applications should set this flag for depth stencil textures if they are never accessed by programmable shaders.
         *
         * @note This flag bit is only valid in combination with the DepthStencilAttachment bit.
        */
        DenyShaderResource = 1 << 6
    };
    LLRI_DEFINE_FLAG_BIT_OPERATORS(resource_usage_flag_bits);
    
    /**
     * @brief Converts a resource_usage_flag_bits to a string.
     * @return The enum value as a string, or "Invalid resource_usage_flag_bits value" if the value was not recognized as an enum member.
    */
    std::string to_string(resource_usage_flag_bits bits);

    /**
     * @brief Describes how the resource will be allowed to be used. Only enable flags that apply to your usage. Enabling flags unnecessarily may result in fewer driver optimizations.
    */
    using resource_usage_flags = flags<resource_usage_flag_bits>;

    /**
     * @brief Converts resource_usage_flags to a string.
     * @return The flags as a string, or "Invalid resource_usage_flags value" if the value was not recognized as a valid combination of resource_usage_flag_bits
    */
    std::string to_string(resource_usage_flags flags);

    /**
     * @brief The type of memory that a resource is allocated with. Different memory types support different operations and may perform better or worse for some operations.
    */
    enum struct resource_memory_type : uint8_t
    {
        /**
         * @brief Optimized for device access. Can not be written to or otherwise viewed by the host. It is recommended to use this for all resources that are used frequently (more than once) by the device.
        */
        Local,
        /**
         * @brief Allows the host to map the resource for writing. Reading is also permitted but is usually much slower than with explicit read resources.
         *
         * Device access for upload resources is slower than with Local resources, and for resources used more than once its recommended to copy over to a device local resource.
         *
         * Resources with this memory type **must** be created with initialState set to resource_state::Upload.
        */
        Upload,
        /**
         * @brief allows the host to map the resource for reading.
         *
         * Device access for read resources is typically much slower than Local resources, consider copying to the read resource instead of using the resource directly.
        */
        Read,
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = Read
    };

    /**
     * @brief Converts a resource_memory_type to a string.
     * @return The enum value as a string, or "Invalid resource_memory_type value" if the value was not recognized as an enum member.
    */
    std::string to_string(resource_memory_type type);

    /**
     * @brief Describes how a resource should be created (its type, size, allocation, usage, etc.)
    */
    struct resource_desc
    {
        /**
         * @brief The device node on which the resource should be created.
         *
         * Exactly one bit **must** be set, and that bit **must** be less than 1 << Adapter::queryNodeCount(). Passing 0 is the equivalent of passing 1.
        */
        uint32_t createNodeMask;
        /**
         * @brief A mask with the device nodes on which the resource will be visible.
         *
         * At least the same bit as createNodeMask **must** be set. Passing 0 is the equivalent of passing 1.
         * Any bits set to 1 in visibleNodeMask **must** be less than 1 << Adapter::queryNodeCount().
        */
        uint32_t visibleNodeMask;

        /**
         * @brief The type of resource.
        */
        resource_type type;
        /**
         * @brief Flags that determine how the resource may be used.
        */
        resource_usage_flags usage;
        /**
         * @brief The type of memory that the resource is created on / optimized for.
        */
        resource_memory_type memoryType;
        /**
         * @brief The state in which the resource should be first after allocation.
        */
        resource_state initialState;

        /**
         * @brief The width of the resource. If the resource is a Buffer then this determines the size of the Buffer in bytes. If the resource is a texture then the width is the number of texels on the x axis.
        */
        uint32_t width;
        /**
         * @brief The number of texels on the y axis of the texture. Ignored if type == resource_type::Buffer or if type == resource_type::Texture1D.
        */
        uint32_t height;
        /**
         * @brief The number of texels on the z axis of the texture. This either determines the depth of a Texture3D, or the number of array layers for all other Texture types. Ignored if type == resource_type::Buffer.
        */
        uint16_t depthOrArrayLayers;
        /**
         * @brief The number of mipmap levels on the texture. Ignored if type == resource_type::Buffer.
        */
        uint16_t mipLevels;

        /**
         * @brief The number of samples per pixel
         * @note Only relevant for Texture resources.
        */
        texture_sample_count sampleCount;
        /**
         * @brief The format of the texture. Ignored if type == resource_type::Buffer.
        */
        texture_format format;
        /**
         * @brief Describes how the texture's texels are stored. Ignored if type == resource_type::Buffer.
        */
        texture_tiling tiling;

        /**
         * @brief Convenience function for creating a buffer resource_desc.
        */
        static constexpr resource_desc buffer(resource_usage_flags usage, resource_memory_type memoryType, resource_state initialState, uint32_t sizeInBytes, uint32_t createNodeMask = 0, uint32_t visibleNodeMask = 0) noexcept;
    };

    class Resource
    {
        friend class Device;

    private:
        // Force private constructor/deconstructor so that only create/destroy can manage lifetime
        Resource() = default;
        ~Resource() = default;

        resource_type m_type;

        void* m_memory = nullptr;
        void* m_resource = nullptr;

        resource_state m_state;
        uint32_t m_implementationState;
    };
}
