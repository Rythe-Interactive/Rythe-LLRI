#pragma once

namespace legion::graphics::llri
{
    enum struct some_strategy_type : unsigned
    {
        naive,
        smart,
        bruteforce
    };

    class Context
    {
    public:
        static unsigned someImplementationAgnosticValue;

        static unsigned createSomeId(some_strategy_type creationStrategy) noexcept;
    };
}
