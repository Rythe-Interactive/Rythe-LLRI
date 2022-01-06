/**
 * @file command_list.inl
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    inline std::string to_string(command_list_usage usage)
    {
        switch(usage)
        {
            case command_list_usage::Direct:
                return "Direct";
            case command_list_usage::Indirect:
                return "Indirect";
        }

        return "Invalid command_list_usage value";
    }

    inline std::string to_string(command_list_state state)
    {
        switch(state)
        {
            case command_list_state::Empty:
                return "Empty";
            case command_list_state::Recording:
                return "Recording";
            case command_list_state::Ready:
                return "Ready";
        }

        return "Invalid command_list_state value";
    }

    inline command_list_alloc_desc CommandList::getDesc() const
    {
        return m_desc;
    }

    inline result CommandList::begin(const command_list_begin_desc& desc)
    {
        LLRI_DETAIL_VALIDATION_REQUIRE(getState() == command_list_state::Empty, result::ErrorInvalidState)
        LLRI_DETAIL_VALIDATION_REQUIRE(m_group->m_currentlyRecording == nullptr, result::ErrorOccupied)

#ifdef LLRI_DETAIL_ENABLE_VALIDATION
        m_group->m_currentlyRecording = this;
#endif

        LLRI_DETAIL_CALL_IMPL(impl_begin(desc), m_validationCallbackMessenger)
    }

    inline result CommandList::end()
    {
        LLRI_DETAIL_VALIDATION_REQUIRE(getState() == command_list_state::Recording, result::ErrorInvalidState)

        m_group->m_currentlyRecording = nullptr;

        LLRI_DETAIL_CALL_IMPL(impl_end(), m_validationCallbackMessenger)
    }

    template<typename Func, typename ...Args>
    result CommandList::record(const command_list_begin_desc& desc, Func&& function, Args&&... args)
    {
        const result r = begin(desc);
        if (r != result::Success)
            return r;

        std::invoke(std::forward<Func>(function), std::forward<Args>(args)...);

        return end();
    }
}
