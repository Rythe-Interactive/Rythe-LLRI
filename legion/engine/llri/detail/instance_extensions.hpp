#pragma once

namespace legion::graphics::llri
{
    /**
     * @brief Describes the kind of instance extension. <br>
     * This value is used in instance_extension and is used internally to recognize which value to pick from instance_extension's union. <br>
     * <br>
     * Instance Extensions aren't guaranteed to be available so use this enum with llri::queryInstanceExtensionSupport() to find out if your desired extension is available prior to adding the extension to your instance desc extension array.
    */
    enum struct instance_extension_type
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
     * @brief Converts an instance_extension_type to a string to aid in debug logging.
    */
    constexpr const char* to_string(const instance_extension_type& result);

    /**
     * @brief Enable or disable API-side validation.
     * API validation checks for parameters and context validity and sends the appropriate messages back if the usage is invalid or otherwise concerning.
    */
    struct api_validation_ext
    {
        bool enable : 1;
    };

    /**
     * @brief Enable or disable GPU-side validation.
     * GPU validation validates shader operations such as buffer read/writes. Enabling this can be useful for debugging but is often associated with a significant cost.
    */
    struct gpu_validation_ext
    {
        bool enable : 1;
    };

    /**
     * @brief Describes an instance extension with its type. <br>
     * instance_extensions aren't guaranteed to be available (hence their name extension), and thus before enabling any instance_extension, you should first query its support with llri::queryInstanceExtensionSupport().
    */
    struct instance_extension
    {
        /**
         * @brief The type of instance extension. <br>
         * This value is used to select the correct union member.
        */
        instance_extension_type type;
        /**
         * @brief The instance extension's information. <br>
         * One of the values in this union must be set, and that value must be the same value as the given type. <br>
         * Passing a different structure than expected causes undefined behaviour and may result in unexplainable result values or internal API errors. <br>
         * All instance extensions are named after their enum entry, followed with EXT or _ext (e.g. instance_extension_type::APIValidation is represented by api_validation_ext).
        */
        union
        {
            api_validation_ext apiValidation;
            gpu_validation_ext gpuValidation;
        };

        instance_extension() = default;
        instance_extension(const instance_extension_type& type, const api_validation_ext& ext) : type(type), apiValidation(ext) { }
        instance_extension(const instance_extension_type& type, const gpu_validation_ext& ext) : type(type), gpuValidation(ext) { }
    };

    namespace detail
    {
        [[nodiscard]] bool queryInstanceExtensionSupport(const instance_extension_type& type);
    }

    /**
     * @brief Queries the support of the given extension.
     * @return true if the extension is supported, and false if it isn't.
     */
    [[nodiscard]] bool queryInstanceExtensionSupport(const instance_extension_type& type);
}
