/**
 * @file math.hpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
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
     * @brief Converts an extent_2d to a string using the format: "{ width, height }"
    */
    inline std::string to_string(extent_2d extent)
    {
        return "{ " + std::to_string(extent.width) + ", " + std::to_string(extent.height) + " }";
    }

    /**
     * @brief A two-dimensional offset described by an x and y coordinate.
    */
    struct offset_2d
    {
        int32_t x;
        int32_t y;
    };

    /**
     * @brief Converts an offset_2d to a string using the format: "{ x, y }"
    */
    inline std::string to_string(offset_2d offset)
    {
        return "{ " + std::to_string(offset.x) + ", " + std::to_string(offset.y) + " }";
    }
}
