/**
 * @file device_extensions.cpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#include <llri/llri.hpp>
#include <doctest/doctest.h>

#include <detail/device_extensions/swapchain_creation.hpp>

TEST_SUITE("Device Extensions")
{
    TEST_CASE("Using device extensions")
    {
        SUBCASE("Swapchain creation")
            testSwapchainCreation();
    }
}
