/**
 * @file adapter.hpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    enum struct adapter_extension : uint8_t;
    enum struct queue_type : uint8_t;
    enum struct format : uint8_t;
    enum struct sample_count : uint8_t;
    enum struct resource_type : uint8_t;
    enum struct present_mode_ext : uint8_t;

    enum struct resource_usage_flag_bits : uint16_t;
    using resource_usage_flags = flags<resource_usage_flag_bits>;

    struct surface_capabilities_ext;

    /**
     * @brief An informational enum describing the type of Adapter. The type does not directly affect how the related adapter operates, but it **may** correlate with performance or the availability of various features.
    */
    enum struct adapter_type : uint8_t
    {
        /**
         * @brief The device type is not recognized as any of the other available types.
         * This may for example be an APU, or a different form of supported processing unit.
        */
        Other,
        /**
         * @brief GPU embedded into the host CPU.
         * Integrated GPUs typically have lower power cost but are (usually) less performant and share their memory with the host system.
        */
        Integrated,
        /**
         * @brief Separate GPU, usually connected to the host system through PCIe connectors.
        */
        Discrete,
        /**
         * @brief The adapter is a virtual node in a virtualization environment
        */
        Virtual,
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = Virtual
    };

    /**
     * @brief Converts an adapter_type to a string.
     * @return The enum value as a string, or "Invalid adapter_type value" if the value was not recognized as an enum member.
    */
    inline std::string to_string(adapter_type type);

    /**
     * @brief Basic information about an adapter.
     */
    struct adapter_info
    {
        /**
         * @brief The unique ID of the hardware vendor. This ID is guaranteed to be the same for all adapters from a given vendor.
        */
        uint32_t vendorId;
        /**
         * @brief The ID of the adapter. This ID refers to the product type/version, meaning that if multiple of the same kind of adapters were to be present, this ID would be the same among all of them.
        */
        uint32_t adapterId;
        /**
         * @brief The name of the adapter. This string describes the adapter and usually includes the vendor name and the product type/version.
        */
        std::string adapterName;
        /**
         * @brief An informational value describing the type of Adapter. The type does not directly affect how the related adapter operates, but it **may** correlate with the availability of various features.
        */
        adapter_type adapterType;
    };

    /**
     * @brief Adapter features describes all features enabled or available to an Adapter.
     * Fill this structure with 0's and enable the features you wish to support, enabling all the supported features could lead to performance loss.
     * Use Adapter::queryFeatures() to get a structure with supported features to test against.
    */
    struct adapter_features
    {
        // reserved for future adapter features.
    };

    /**
     * @brief Adapter limits describes the limits of the Adapter.
    */
    struct adapter_limits
    {
        // reserved for future adapter limits.
    };

    /**
     * @brief Describes a format's properties.
    */
    struct format_properties
    {
        /**
         * @brief If the format is supported by the selected adapter at all.
        */
        bool supported;
        /**
         * @brief If the format is supported for each resource_type.
         * @note resource_type::Buffer is present in this map for completeness but is always set to false.
        */
        std::unordered_map<resource_type, bool> types;
        /**
         * @brief The resource usage flag bits that are supported when this format is used.
        */
        resource_usage_flags usage;
        /**
         * @brief If the format supports multi-sampling for each sample_count value.
        */
        std::unordered_map<sample_count, bool> sampleCounts;
    };

    /**
     * @brief Represents a compatible adapter (GPU, APU, IGPU, etc.).
     * This handle is created and owned by the Instance, the user is not responsible for destroying it.
    */
    class Adapter
    {
        friend class Instance;
        friend class Device;
        friend result detail::impl_createInstance(const instance_desc&, Instance**, bool);
        friend void detail::impl_destroyInstance(Instance*);
        
    public:
        using native_adapter = void;
        
        /**
         * @brief Gets the native Adapter pointer, which depending on the llri::getImplementation() is a pointer to the following:
         *
         * DirectX12: IDXGIAdapter*
         * Vulkan: VkPhysicalDevice
         */
        [[nodiscard]] native_adapter* getNative() const;
        
        /**
         * @brief Query basic information about the Adapter.
        */
        [[nodiscard]] adapter_info queryInfo() const;

        /**
         * @brief Query a structure with all supported driver/hardware features.
        */
        [[nodiscard]] adapter_features queryFeatures() const;

        /**
         * @brief Query a structure with information about the Adapter's limits.
        */
        [[nodiscard]] adapter_limits queryLimits() const;

        /**
         * @brief Query the support of a given adapter extension.
         * @param ext The type of adapter extension to check against.
         *
         * @note Always returns false if ext > adapter_extension::MaxEnum.
         */
        [[nodiscard]] bool queryExtensionSupport(adapter_extension ext) const;
        
        /**
         * @brief Queries if the queue type can present to the Surface.
         *
         * queue_type::Graphics is most commonly able to present, queue_type::Compute is sometimes able to present (never on DirectX12), and queue_type::Transfer can never present.
         *
         * @note Valid usage (ErrorExtensionNotEnabled): at least one instance_extension prefixed with Surface **must** be enabled.
         * @note Valid usage (ErrorInvalidUsage): surface **must** be a valid non-null pointer to a SurfaceEXT object.
         * @note Valid usage (ErrorInvalidUsage): type **must** be less or equal to queue_type::MaxEnum.
         * @note Valid usage (ErrorInvalidUssage): support **must** be a valid non-null pointer to a boolean variable.
         *
         * @return Success upon correct excecution of the operation.
         * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory, ErrorSurfaceLostEXT.
         */
        result querySurfacePresentSupportEXT(SurfaceEXT* surface, queue_type type, bool* support) const;

        /**
         * @brief Query the Surface's capabilities to determine what formats, present modes, etc. a swapchain with this surface could support.
         *
         * @note Valid usage (ErrorExtensionNotEnabled): at least one instance_extension prefixed with Surface **must** be enabled.
         * @note Valid usage (ErrorInvalidUsage): capabilities **must** be a valid non-null pointer to a surface_capabilities_ext variable
         * @note Valid usage (ErrorInvalidUsage): surface **must** be a valid non-null pointer to a SurfaceEXT object.
         *
         * @return Success upon correct execution of the operation.
         * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory, ErrorSurfaceLostEXT.
        */
        result querySurfaceCapabilitiesEXT(SurfaceEXT* surface, surface_capabilities_ext* capabilities) const;

        /**
         * @brief Query the maximum number of available queues for a given queue type.
         * @param type The type of queue. This must be a valid queue_type value, or the function returns 0.
         *
         * @note (Device nodes) Queues are shared across device nodes. The Queue selects nodes (Adapters) to execute the commands on based on command list parameters.
        */
        [[nodiscard]] uint8_t queryQueueCount(queue_type type) const;

        /**
         * @brief Query the properties of all formats.
         * The resulting unordered_map contains a format_properties structure for every format in llri::format.
         *
         * The result of queryFormatProperties is cached internally and a reference is returned. Thus calling the function multiple times does not come with an extra cost and calling queryFormatProperties(format) doesn't either.
        */
        [[nodiscard]] const std::unordered_map<format, format_properties>& queryFormatProperties() const;

        /**
         * @brief Query the properties of a single format.
         */
        [[nodiscard]] format_properties queryFormatProperties(format f) const;

        /**
         * @brief Query the number of nodes (physical adapters) that this adapter represents. If there are no linked physical adapters, this returns 1.
        */
        [[nodiscard]] uint8_t queryNodeCount() const;

    private:
        // Force private constructor/deconstructor so that only instance can manage lifetime
        Adapter() = default;
        ~Adapter() = default;

        native_adapter* m_ptr = nullptr;
        uint8_t m_nodeCount = 1;

        Instance* m_instance = nullptr;

        void* m_validationCallbackMessenger = nullptr;

        // cached value of queryFormatProperties()
        mutable std::unordered_map<format, format_properties> m_cachedFormatProperties {};

        [[nodiscard]] adapter_info impl_queryInfo() const;
        [[nodiscard]] adapter_features impl_queryFeatures() const;
        [[nodiscard]] adapter_limits impl_queryLimits() const;
        [[nodiscard]] bool impl_queryExtensionSupport(adapter_extension ext) const;

        [[nodiscard]] uint8_t impl_queryQueueCount(queue_type type) const;
        [[nodiscard]] std::unordered_map<format, format_properties> impl_queryFormatProperties() const;
        
        result impl_querySurfacePresentSupportEXT(SurfaceEXT* surface, queue_type type, bool* support) const;
        result impl_querySurfaceCapabilitiesEXT(SurfaceEXT* surface, surface_capabilities_ext* capabilities) const;
    };
}
