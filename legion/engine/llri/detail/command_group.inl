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

    inline result CommandGroup::allocate(const command_list_alloc_desc& desc, CommandList** cmdList)
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (cmdList == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "CommandGroup::allocate() returned ErrorInvalidUsage because the passed cmdList parameter was nullptr");
            return result::ErrorInvalidUsage;
        }
#endif

        *cmdList = nullptr;

#ifndef LLRI_DISABLE_VALIDATION
        if (desc.usage > command_list_usage::MaxEnum)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "CommandGroup::allocate() return ErrorInvalidUsage because desc.usage is not a valid enum value");
            return result::ErrorInvalidUsage;
        }

        if (m_cmdLists.size() + 1 > m_maxCount)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "CommandGroup::allocate() returned ErrorExceededLimit because allocating 1 CommandList from the group would exceed the CommandGroup's maximum number of allocated CommandLists");
            return result::ErrorInvalidUsage;
        }
#endif

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_allocate(desc, cmdList);
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
        return r;
#else
        return impl_allocate(desc, cmdList);
#endif
    }

    inline result CommandGroup::allocate(const command_list_alloc_desc& desc, uint8_t count, std::vector<CommandList*>* cmdLists)
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (cmdLists == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "CommandGroup::allocate() returned ErrorInvalidUsage because the passed cmdLists parameter was nullptr");
            return result::ErrorInvalidUsage;
        }
#endif

        cmdLists->clear();

#ifndef LLRI_DISABLE_VALIDATION
        if (desc.usage > command_list_usage::MaxEnum)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "CommandGroup::allocate() return ErrorInvalidUsage because desc.usage is not a valid enum value");
            return result::ErrorInvalidUsage;
        }
        
        if (count == 0)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "CommandGroup::allocate() returned ErrorInvalidUsage because count was 0");
            return result::ErrorInvalidUsage;
        }

        if (m_cmdLists.size() + count > m_maxCount)
        {
            const std::string str = std::string("CommandGroup::allocate() returned ErrorExceededLimit because the CommandGroup currently has ") + std::to_string(m_cmdLists.size()) + " CommandLists allocated, and allocating " + std::to_string(count) + " more CommandLists from the group would exceed the CommandGroup's maximum number of allocated CommandLists (" + std::to_string(m_maxCount) + ").";
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, str.c_str());
            return result::ErrorInvalidUsage;
        }
#endif

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_allocate(desc, count, cmdLists);
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
        return r;
#else
        return impl_allocate(desc, count, cmdLists);
#endif
    }

    inline result CommandGroup::free(CommandList* cmdList)
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (cmdList == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "CommandGroup::free() returned ErrorInvalidUsage because the passed cmdList parameter was nullptr");
            return result::ErrorInvalidUsage;
        }

        if (std::find(m_cmdLists.begin(), m_cmdLists.end(), cmdList) == m_cmdLists.end())
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "CommandGroup::free() returned ErrorInvalidUsage because the passed CommandList wasn't allocated through the passed CommandGroup");
            return result::ErrorInvalidUsage;
        }
#endif

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_free(cmdList);
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
        return r;
#else
        return impl_free(cmdList);
#endif
    }

    inline result CommandGroup::free(uint8_t numCommandLists, CommandList** cmdLists)
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (numCommandLists == 0)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "CommandGroup::free() returned ErrorInvalidUsage because numCommandLists was 0");
            return result::ErrorInvalidUsage;
        }

        if (cmdLists == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "CommandGroup::free() returned ErrorInvalidUsage because the passed cmdLists parameter was nullptr");
            return result::ErrorInvalidUsage;
        }

        for (uint8_t i = 0; i < numCommandLists; i++)
        {
            if (cmdLists[i] == nullptr)
            {
                const std::string str = "CommandGroup::free() returned ErrorInvalidUsage because cmdLists member " + std::to_string(i) + " was nullptr";
                m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, str.c_str());
                return result::ErrorInvalidUsage;
            }

            if (std::find(m_cmdLists.begin(), m_cmdLists.end(), cmdLists[i]) == m_cmdLists.end())
            {
                const std::string str = "CommandGroup::free() returned ErrorInvalidUsage because cmdLists member " + std::to_string(i) + " wasn't allocated through the CommandGroup";
                m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, str.c_str());
                return result::ErrorInvalidUsage;
            }
        }
#endif

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_free(numCommandLists, cmdLists);
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
        return r;
#else
        return impl_free(numCommandLists, cmdLists);
#endif
    }
}
