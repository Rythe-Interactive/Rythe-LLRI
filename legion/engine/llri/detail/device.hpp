/**
 * Copyright 2021-2021 Leon Brands. All rights served.
 * License: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
//detail includes should be kept to a minimum but
//are allowed as long as dependencies are upwards (e.g. device may include adapter but not vice versa)
#include <llri/detail/adapter.hpp>

namespace LLRI_NAMESPACE
{
    /**
     * @brief Device description to be used in Instance::createDevice().
    */
    struct device_desc
    {
        /**
         * @brief The adapter to create the instance for.
        */
        Adapter* adapter;
        /**
         * @brief The enabled adapter features.
         * You should only enable features that you'll need because enabling features can disable driver optimizations.
        */
        adapter_features features;
        /**
         * @brief The number of device extensions in the device_desc::extensions array.
        */
        uint32_t numExtensions;
        /**
         * @brief The device extensions, if device_desc::numExtensions > 0, then this has to be a valid pointer to an array of DeviceExtension.
         * If numExtensions == 0, then this pointer may be nullptr.
        */
        adapter_extension* extensions;
    };

    /**
     * @brief A device is a virtual representation of an adapter and can create/destroy resources for the said adapter.
     */
    class Device
    {
        friend Instance;
    public:

    private:
        //Force private constructor/deconstructor so that only create/destroy can manage lifetime
        Device() = default;
        ~Device() = default;

        void* m_ptr = nullptr;
        void* m_functionTable = nullptr;

        validation_callback_desc m_validationCallback;
        void* m_validationCallbackMessenger = nullptr;
    };
}
