/**
 * @file llri.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>

namespace LLRI_NAMESPACE
{
    [[nodiscard]] implementation queryImplementation()
    {
        return implementation::DirectX12;
    }
}
