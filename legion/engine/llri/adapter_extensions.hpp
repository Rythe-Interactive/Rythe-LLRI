#pragma once

namespace legion::graphics::llri
{
    /**
     * @brief Describes the kind of adapter extension. <br>
     * This value is used in AdapterExtension and is used internally to recognize the extension type and
     * select the correct value from the AdapterExtenision's union. <br>
     * <br>
     * Adapter Extensions aren't guaranteed to be available so use this enum with Adapter::queryExtensionSupport() to find out if your desired extension is available prior to adding the extension to your desc extension array.
    */
    enum class AdapterExtensionType
    {

    };

    /**
     * @brief Converts an AdapterExtensionType to a string to aid in debug logging.
    */
    constexpr const char* to_string(const AdapterExtensionType& type)
    {
        switch (type)
        {
            default:
                break;
        }

        return "Invalid AdapterType value";
    }

    /**
     * @brief Describes an instance extension with its type. <br>
     * <br>
     * Adapter Extensions aren't guaranteed to be available so query their availability with Adapter::queryExtensionSupport() to find out if your desired extension is available prior to adding the extension to the desc extension array.
    */
    struct AdapterExtension
    {
        AdapterExtensionType type;

        union
        {
            //Empty until adapter extensions are added
        };

        AdapterExtension() = default;
    };
}
