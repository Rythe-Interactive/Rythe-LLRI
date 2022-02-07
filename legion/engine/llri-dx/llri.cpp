/**
 * @file llri.cpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#include <llri/llri.hpp>

namespace llri
{
    [[nodiscard]] implementation getImplementation()
    {
        return implementation::DirectX12;
    }
}
