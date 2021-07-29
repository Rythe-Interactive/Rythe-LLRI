#pragma once
#include <llri/llri.hpp> //Recursive include technically not necessary but helps with intellisense

namespace legion::graphics::llri
{
    constexpr inline const char* to_string(const result& r)
    {
        switch (r)
        {
        case result::Success:
            return "Success";
        case result::Timeout:
            return "Timeout";
        case result::ErrorUnknown:
            return "ErrorUnknown";
        case result::ErrorInvalidUsage:
            return "ErrorInvalidUsage";
        case result::ErrorFeatureNotSupported:
            return "ErrorFeatureNotSupported";
        case result::ErrorExtensionNotSupported:
            return "ErrorExtensionNotSupported";
        case result::ErrorDeviceHung:
            return "ErrorDeviceHung";
        case result::ErrorDeviceLost:
            return "ErrorDeviceLost";
        case result::ErrorDeviceRemoved:
            return "ErrorDeviceRemoved";
        case result::ErrorDriverFailure:
            return "ErrorDriverFailure";
        case result::NotReady:
            return "NotReady";
        case result::ErrorOutOfHostMemory:
            return "ErrorOutOfHostMemory";
        case result::ErrorOutOfDeviceMemory:
            return "ErrorOutOfDeviceMemory";
        case result::ErrorInitializationFailed:
            return "ErrorInitializationFailed";
        case result::ErrorIncompatibleDriver:
            return "ErrorIncompatibleDriver";
        }

        return "Invalid result value";
    }
}

#include <llri/detail/instance.inl>
#include <llri/detail/adapter.inl>
#include <llri/detail/device.inl>
