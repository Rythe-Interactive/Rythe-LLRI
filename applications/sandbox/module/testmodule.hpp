#pragma once
#include <core/core.hpp>

#include "../systems/testsystem.hpp"

/**
 * @file testmodule.hpp
 */


/**@class TestModule
 * @brief Custom module.
 */
class TestModule final : public lgn::Module
{
public:
    /**@brief Will automatically be called once before the start of the application.
     */
    virtual void setup()
    {
        // Here you can report any custom components and systems.
        // (components don't always need ot be reported.)
        reportSystem<TestSystem>();
    }

    /**@brief This function is used to decide the order in which to call the setup function.
     */
    virtual lgn::priority_type priority() override
    {
        return default_priority;
    }
};
