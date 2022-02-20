/**
 * @file swapchain_ext.hpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    /**
     * @brief Describes how the presentation engine interacts with a swapchain's textures and the surface.
    */
    enum struct present_mode_ext : uint8_t
    {
        /**
         * @brief The presentation engine doesn't wait for vertical sync, which may result in visible tearing.
        */
        Immediate,
        /**
         * @brief The presentation engine waits for vertical sync before swapping/presenting textures.
        */
        Fifo,
        /**
         * @brief The presentation engine waits for vertical sync before swapping textures internally, but enables swapping between the textures in queue that are waiting to be presented, resulting in no application wait time.
        */
        Mailbox,
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = Mailbox
    };

    /**
     * @brief Converts a present_mode_ext to a string.
     * @return The enum value as a string, or "Invalid present_mode_ext value" if the value was not recognized as an enum member.
    */
    inline std::string to_string(present_mode_ext mode);

    /**
     * @brief Describes the SurfaceEXT's capabilities for SwapchainEXT creation.
     *
     * SwapchainEXT **must** be created with this structure in mind, usually only a limited set of capabilities are available (e.g. swapchain texture formats).
    */
    struct surface_capabilities_ext
    {
        /**
         * @brief The minimum number of textures (inclusive).
        */
        uint32_t minTextureCount;
        /**
         * @brief The maximum number of textures (inclusive).
        */
        uint32_t maxTextureCount;

        /**
         * @brief The minimum texture extent (inclusive).
        */
        extent_2d minTextureExtent;
        /**
         * @brief The maximum texture extent (inclusive).
        */
        extent_2d maxTextureExtent;

        /**
         * @brief The supported texture formats.
        */
        std::vector<format> textureFormats;

        /**
         * @brief The supported swapchain present modes.
        */
        std::vector<present_mode_ext> presentModes;

        /**
         * @brief The supported texture usage flags bits
        */
        resource_usage_flags textureUsage;
    };

    /**
     * @brief Describes how a swapchain should be created.
     * Swapchain creation should adhere to the limits of the SurfaceEXT that it is created for.
     * These limits are described in a surface_capabilities structure, queryable through Adapter::querySurfaceCapabilities().
    */
    struct swapchain_desc_ext
    {
        /**
         * @brief The queue on which the swapchain will present.
         *
         * @note Valid usage (ErrorInvalidUsage): queue **must not** be nullptr.
         * @note Valid usage (ErrorInvalidUsage): queue->getDesc().type **must** be supported in Adapter::querySurfacePresentSupportEXT()
        */
        Queue* queue;
        /**
         * @brief The surface to create a swapchain for.
         *
         * @note Valid usage (ErrorInvalidUsage): surface **must not** be nullptr.
        */
        SurfaceEXT* surface;
        
        /**
         * @brief Describes how the presentation engine handles swapping buffers.
         *
         * @note Valid usage (ErrorInvalidUsage): presentMode **must** be less than or equal to present_mode_ext::MaxEnum.
         * @note Valid usage (ErrorInvalidUsage): presentMode **must** be an element of surface_capabilities::presentModes.
        */
        present_mode_ext presentMode;

        /**
         * @brief The minimum number of textures in the swapchain.
         *
         * Implementations **may** decide to create more textures than requested.
         *
         * @note Valid usage (ErrorInvalidUsage): textureCount **must** be more than 0.
         * @note Valid usage (ErrorInvalidUsage): textureCount **must** be more than or equals to surface_capabilities::minTextureCount.
         * @note Valid usage (ErrorInvalidUsage): textureCount **must** be less than or equals to surface_capabilities::maxTextureCount.
        */
        uint32_t textureCount;

        /**
         * @brief The extent of the Swapchain's textures.
         *
         * @note Valid usage (ErrorInvalidUsage): textureExtent.width **must not** be less than surface_capabilities::minExtent::width
         * @note Valid usage (ErrorInvalidUsage): textureExtent.height **must not** be less than surface_capabilities::minExtent::height
         *
         * @note Valid usage (ErrorInvalidUsage): textureExtent.width **must not** be more than surface_capabilities::maxExtent::width
         * @note Valid usage (ErrorInvalidUsage): textureExtent.height **must not** be more than surface_capabilities::maxExtent.height
        */
        extent_2d textureExtent;

        /**
         * @brief The format of the Swapchain's textures.
         *
         * @note Valid usage (ErrorInvalidUsage): textureFormat **must** be less than or equal to format::MaxEnum.
         * @note Valid usage (ErrorInvalidUsage): textureFormat **must** be an element of  surface_capabilities::formats.
        */
        format textureFormat;
        
        /**
         * @brief Describes how the Swapchain's textures will be used.
         *
         * @note Valid usage (ErrorInvalidUsage): textureUsage **must** be a valid combination of resource_usage_flag_bits.
         * @note Valid usage (ErrorInvalidUsage): textureUsage **must not** be resource_usage_flag_bits::None.
         * @note Valid usage (ErrorInvalidUsage): each enabled bit in textureUsage **must** also be enabled in surface_capabilities::usageBits.
        */
        resource_usage_flags textureUsage;
    };

    /**
     * @brief Swapchains provide the ability to swap or loop textures between the application and the surface.
    */
    class SwapchainEXT
    {
        friend class Device;

    public:
        using native_swapchain_ext = void;
        
        /**
         * @brief Get the desc that the SwapchainEXT was created with.
         */
        [[nodiscard]] swapchain_desc_ext getDesc() const;

        /**
         * @brief Gets the native SwapchainEXT pointer, which depending on the llri::getImplementation() is a pointer to the following:
         *
         * DirectX12: IDXGISwapchain*
         * Vulkan: VkSwapchainKHR
         */
        [[nodiscard]] native_swapchain_ext* getNative() const;
        
    private:
        // Force private constructor/deconstructor so that only create/destroy can manage lifetime
        SwapchainEXT() = default;
        ~SwapchainEXT() = default;

        void* m_ptr = nullptr;
        Device* m_device = nullptr;
        swapchain_desc_ext m_desc;
    };
}
