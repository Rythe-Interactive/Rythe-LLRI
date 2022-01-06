/**
 * @file llri.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
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
