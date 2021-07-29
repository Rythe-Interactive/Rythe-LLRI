#pragma once
//detail includes should be kept to a minimum but
//are allowed as long as dependencies are upwards (e.g. adapter may include instance but not vice versa)
#include <llri/detail/instance.hpp>

namespace legion::graphics::llri
{
    enum struct adapter_extension_type;

    /**
     * @brief An informational enum describing the type of Adapter. The type does not directly affect how the related adapter operates, but it may correlate with performance or the availability of various features.
    */
    enum struct adapter_type
    {
        /**
         * @brief The device type is not recognized as any of the other available types.
         * This may for example be an APU, or a different form of Vulkan supported processing unit.
        */
        Other,
        /**
         * @brief GPU embedded into the host CPU.
         * Integrated GPUs typically have lower power cost but are (usually) less performant and share their memory with the host system.
        */
        Integrated,
        /**
         * @brief Separate GPU, usually connected to the host system through PCIE connectors.
        */
        Discrete,
        /**
         * @brief The adapter is a virtual node in a virtualization environment
        */
        Virtual
    };

    /**
     * @brief Converts an adapter_type to a string to aid in debug logging.
    */
    constexpr const char* to_string(const adapter_type& type);

    /**
     * @brief Basic information about an adapter.
     * This information is for informative purposes only and the results aren't guaranteed to be the same across APIs.
     */
    struct adapter_info
    {
        /**
         * @brief The unique ID of the hardware vendor (e.g. NVIDIA, Intel or AMD).
        */
        uint32_t vendorId;
        /**
         * @brief The ID of the adapter. This ID refers to the product type/version, meaning that if multiple of the same kind of adapters are present, this ID will be the same among them.
        */
        uint32_t adapterId;
        /**
         * @brief The name of the adapter. This string describes the adapter and usually includes the vendor name and the product type/version.
        */
        std::string adapterName;
        /**
         * @brief An informational value describing the type of Adapter. The type does not directly affect how the related adapter operates, but it may correlate with the availability of various features.
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

    };

    /**
     * @brief A compatible adapter (GPU, APU, IGPU, etc.).
     * This handle is created and owned by the Instance, the user is not responsible for destroying it.
    */
    class Adapter
    {
        friend Instance;
        friend result detail::impl_createInstance(const instance_desc&, Instance**, const bool&);
        friend void detail::impl_destroyInstance(Instance*);

    public:
        /**
         * @brief Get basic information about the Adapter. Do note that this information should be for informative purposes only and the results aren't guaranteed to be the same across APIs.
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if info is nullptr.
         * @return ErrorDeviceLost If the adapter was removed or lost.
        */
        result queryInfo(adapter_info* info) const;

        /**
         * @brief Get a structure with all supported driver/hardware features.
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if features is nullptr.
         * @return ErrorIncompatibleDriver if the Adapter doesn't support the backend's requested API (either Vulkan 1.0 or DirectX 12 depending on the build).
         * @return ErrorDeviceLost If the adapter was removed or lost.
        */
        result queryFeatures(adapter_features* features) const;

        /**
         * @brief Get the support of a given adapter extension.
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage If supported is nullptr.
         * @return ErrorDeviceLost If the adapter was removed or lost.
         */
        result queryExtensionSupport(const adapter_extension_type& type, bool* supported) const;
    private:
        //Force private constructor/deconstructor so that only instance can manage lifetime
        Adapter() = default;
        ~Adapter() = default;

        void* m_ptr = nullptr;
        validation_callback_desc m_validationCallback;
        void* m_validationCallbackMessenger = nullptr;

        result impl_queryInfo(adapter_info* info) const;
        result impl_queryFeatures(adapter_features* features) const;
        result impl_queryExtensionSupport(const adapter_extension_type& type, bool* supported) const;
    };
}
