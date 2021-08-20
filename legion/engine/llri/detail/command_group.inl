/**
 * @file command_group.inl
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> //Recursive include technically not necessary but helps with intellisense

namespace LLRI_NAMESPACE
{
    inline result CommandGroup::reset()
    {
#ifndef LLRI_DISABLE_VALIDATION
        for (auto* cmdList : m_cmdLists)
        {
            if (cmdList->queryState() == command_list_state::Recording)
            {
                const std::string str = "CommandGroup::reset() returned ErrorInvalidState because CommandList " + std::to_string((int)cmdList) + " was in the command_list_state::Recording state.";
                m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, str.c_str());
                return result::ErrorInvalidState;
            }
        }
#endif

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_reset();
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
        return r;
#else
        return impl_reset();
#endif
    }
}
