/**
 * Copyright 2021-2021 Leon Brands. All rights served.
 * License: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <core/core.hpp>

#include "../systems/testsystem.hpp"

class TestModule final : public lgn::Module
{
public:
    void setup() override
    {
        reportSystem<TestSystem>();
    }

    lgn::priority_type priority() override
    {
        return default_priority;
    }
};
