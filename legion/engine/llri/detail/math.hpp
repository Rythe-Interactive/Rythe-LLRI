/**
 * @file math.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <cstdint>

namespace llri
{
    /**
     * @brief A two-dimensional extent described by a width and height.
    */
    struct extent_2d
    {
        uint32_t width;
        uint32_t height;
    };

    /**
     * @brief A two-dimensional offset described by an x and y coordinate.
    */
    struct offset_2d
    {
        int32_t x;
        int32_t y;
    };
}
