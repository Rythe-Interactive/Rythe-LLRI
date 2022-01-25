/**
 * @file llri.inl
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    inline std::string to_string(result r)
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
            case result::ErrorExtensionNotEnabled:
                return "ErrorExtensionNotEnabled";
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
            case result::ErrorInvalidState:
                return "ErrorInvalidState";
            case result::ErrorExceededLimit:
                return "ErrorExceededLimit";
            case result::ErrorInvalidNodeMask:
                return "ErrorInvalidNodeMask";
            case result::ErrorIncompatibleNodeMask:
                return "ErrorIncompatibleNodeMask";
            case result::ErrorOccupied:
                return "ErrorOccupied";
            case result::ErrorNotSignaled:
                return "ErrorNotSignaled";
            case result::ErrorAlreadySignaled:
                return "ErrorAlreadySignaled";
            case result::ErrorInvalidFormat:
                return "ErrorInvalidFormat";
            case result::ErrorSurfaceLostEXT:
                return "ErrorSurfaceLostEXT";
        }

        return "Invalid result value";
    }

    inline std::string to_string(implementation impl)
    {
        switch (impl)
        {
            case implementation::Vulkan:
                return "Vulkan";
            case implementation::DirectX12:
                return "DirectX12";
        }

        return "Invalid implementation value";
    }
}

#include <llri/detail/callback.inl>

#include <llri/detail/instance.inl>
#include <llri/detail/adapter.inl>

#include <llri/detail/queue.inl>
#include <llri/detail/device.inl>

#include <llri/detail/resource.inl>

#include <llri/detail/command_group.inl>
#include <llri/detail/command_list.inl>

#include <llri/detail/fence.inl>

#include <llri/detail/surface_ext.inl>
