#include <llri/llri.h>

namespace legion::graphics::llri
{
    namespace detail
    {
        static constexpr unsigned VKStaticId = 42;
    }

    unsigned llri::Context::createSomeId(some_strategy_type creationStrategy) noexcept
    {
        return detail::VKStaticId + static_cast<unsigned>(creationStrategy);
    }
}
