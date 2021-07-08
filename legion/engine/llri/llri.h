#pragma once
#include <core/core.hpp>

namespace legion::graphics::llri
{
    enum struct some_strategy_type : id_type
    {
        naive,
        smart,
        bruteforce
    };

    class Context
    {
    public:
        static size_type someImplementationAgnosticValue;

        static id_type createSomeId(some_strategy_type creationStrategy) noexcept;
    };
}
