#include <llri/llri.h>

namespace legion::graphics::llri
{
    namespace detail
    {
        static constexpr id_type VKStaticId = 42;
    }

    id_type llri::Context::createSomeId(some_strategy_type creationStrategy) noexcept
    {
        log::info("This is the VK implementation!");
        return detail::VKStaticId + static_cast<id_type>(creationStrategy);
    }
}
