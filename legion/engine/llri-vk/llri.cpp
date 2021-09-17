/**
 * @file llri.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>

#define VOLK_IMPLEMENTATION
#include <graphics/vulkan/volk.h>

namespace LLRI_NAMESPACE
{
    [[nodiscard]] implementation queryImplementation()
    {
        return implementation::Vulkan;
    }
}
