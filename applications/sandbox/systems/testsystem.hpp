#pragma once
#include <core/core.hpp>
#include <llri/llri.h>

/**
 * @file testsystem.hpp
 */

/**@class TestSystem
 * @brief Custom system.
 */
class TestSystem final : public lgn::System<TestSystem>
{
public:
    /**@brief Will automatically be called once at the start of the application.
     */
    void setup();

    /**@brief Default process marked in setup to run as fast as possible on the "Update" interval.
     */
    void update(lgn::time::span deltaTime);
};
