#pragma once

namespace LLRI_NAMESPACE
{
    /**
     * @brief Describes the kind of adapter extension. <br>
     * This value is used in adapter_extension and is used in :func:`llri::Instance::createDevice()` to recognize the extension type and
     * select the correct value from the adapter_extension's union. <br>
     * <br>
     * Adapter Extensions aren't guaranteed to be available so use this enum with Adapter::queryExtensionSupport() to find out if your desired extension is available prior to adding the extension to your desc extension array.
    */
    enum struct adapter_extension_type
    {
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = 0
    };

    /**
     * @brief Converts an adapter_extension_type to a string to aid in debug logging.
    */
    constexpr const char* to_string(const adapter_extension_type& type)
    {
        switch (type)
        {
            default:
                break;
        }

        return "Invalid adapter_extension_type value";
    }

    /**
     * @brief Describes an instance extension with its type. <br>
     * <br>
     * Adapter Extensions aren't guaranteed to be available so query their availability with Adapter::queryExtensionSupport() to find out if your desired extension is available prior to adding the extension to the desc extension array.
    */
    struct adapter_extension
    {
        adapter_extension_type type;

        union
        {
            //Empty until adapter extensions are added
        };

        adapter_extension() = default;
    };
}
