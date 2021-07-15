#include <llri/llri.h>

namespace legion::graphics::llri
{
    namespace detail
    {
        static constexpr unsigned DXStaticId = 13;
    }

    unsigned llri::Context::createSomeId(some_strategy_type creationStrategy) noexcept
    {
        return detail::DXStaticId - static_cast<unsigned>(creationStrategy);
    }
}
