#include "testsystem.hpp"
#include <llri/llri.h>

void TestSystem::setup()
{
    using namespace legion::graphics::llri;
    using namespace legion;
    log::filter(log::severity_debug);
    log::debug(Context::createSomeId(some_strategy_type::bruteforce));
}

void TestSystem::update(lgn::time::span deltaTime)
{
    // This will be called every frame during "Update".

    //static auto filter = createFilter<component0, component1>();
    //for(auto ent : filter)
    //  Do things.
}
