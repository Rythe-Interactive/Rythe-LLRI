/**
 * @file llri.cpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#include <llri/llri.hpp>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_CASE("getImplementation()")
{
    SUBCASE("")
    {
        CHECK_UNARY(llri::getImplementation() <= llri::implementation::MaxEnum);
    }
}
