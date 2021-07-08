#include <llri/llri.h>

namespace legion::graphics::llri
{
    namespace detail
    {
        static constexpr id_type DXStaticId = 13;
    }

    id_type llri::Context::createSomeId(some_strategy_type creationStrategy) noexcept
    {
        log::info("This is the DX implementation!");
        return detail::DXStaticId - static_cast<id_type>(creationStrategy);
    }
}
