#pragma once

namespace legion::graphics::llri
{
    /**
     * @brief Describes the kind of instance extension. <br>
     * This value is used in InstanceExtension and is used internally to recognize the extension type and
     * cast InstanceExtension::data to the correct underlying type (e.g. APIValidationEXT). <br>
     * <br>
     * Instance Extensions aren't guaranteed to be available so use this enum with llri::queryInstanceExtensionSupport() to find out if your desired extension is available prior to adding the extension to your instance desc extension array.
    */
    enum class InstanceExtensionType
    {
        /**
         * @brief Validate API calls, their parameters, and context.
         */
        APIValidation,
        /**
         * @brief Validate shader operations such as buffer reads/writes.
        */
        GPUValidation
    };

    /**
     * @brief Converts an InstanceExtensionType to a string to aid in debug logging.
    */
    constexpr const char* to_string(const InstanceExtensionType& result)
    {
        switch (result)
        {
            case InstanceExtensionType::APIValidation:
                return "APIValidation";
            case InstanceExtensionType::GPUValidation:
                return "GPUValidation";
        }

        return "Invalid Result value";
    }

    /**
     * @brief Describes an instance extension with its type. <br>
     * InstanceExtensions aren't guaranteed to be available (hence their name Extension), and thus before enabling any InstanceExtension, you should first query its support with llri::queryInstanceExtensionSupport().
    */
    struct InstanceExtension
    {
        /**
         * @brief The type of instance extension. <br>
         * This value is used to cast data to the correct underlying type.
        */
        InstanceExtensionType type;
        /**
         * @brief A pointer to an instance extension struct. <br>
         * The pointer must NOT be nullptr, and must be a valid pointer to the type's corresponding extension structure. <br>
         * Passing a different structure than expected causes undefined behaviour and may result in unexplainable result values or internal API errors. <br>
         * Make sure that the pointer you're passing is still valid upon createInstance() usage and has not been deleted or become out of scope. <br>
         * All instance extensions are named after their enum entry, followed with EXT (e.g. InstanceExtensionType::APIValidation is represented by APIValidationEXT).
        */
        void* data;

        InstanceExtension() = default;
        explicit InstanceExtension(const InstanceExtensionType& type, void* data) : type(type), data(data) { }
    };

    /**
     * @brief Queries the support of the given extension. Returns true if the extension is supported, and false if it isn't.
     */
    [[nodiscard]] bool queryInstanceExtensionSupport(const InstanceExtensionType& type);

    /**
     * @brief Enable or disable API-side validation.
     * API validation checks for parameters and context validity and sends the appropriate messages back if the usage is invalid or otherwise concerning.
    */
    struct APIValidationEXT
    {
        bool enable : 1;

        APIValidationEXT() = default;
        explicit APIValidationEXT(const bool& enable) : enable(enable) { }
    };

    /**
     * @brief Enable or disable GPU-side validation.
     * GPU validation validates shader operations such as buffer read/writes. Enabling this can be useful for debugging but is often associated with a significant cost.
    */
    struct GPUValidationEXT
    {
        bool enable : 1;

        GPUValidationEXT() = default;
        explicit GPUValidationEXT(const bool& enable) : enable(enable) { }
    };
}
