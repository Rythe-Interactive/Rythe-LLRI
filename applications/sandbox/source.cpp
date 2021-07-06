#define LEGION_ENTRY
#if defined(NDEBUG)
#define LEGION_KEEP_CONSOLE
#endif

#include <core/core.hpp>

#include "module/testmodule.hpp"

void LEGION_CCONV reportModules(lgn::Engine* engine)
{
    engine->reportModule<TestModule>();
}
