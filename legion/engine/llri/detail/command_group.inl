/**
 * @file command_group.inl
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    inline queue_type CommandGroup::getType() const
    {
        return m_type;
    }

    inline result CommandGroup::reset()
    {
#ifdef LLRI_DETAIL_ENABLE_VALIDATION
        for (auto* cmdList : m_cmdLists)
        {
            LLRI_DETAIL_VALIDATION_REQUIRE(cmdList->queryState() != command_list_state::Recording, result::ErrorInvalidState)
        }
#endif

        LLRI_DETAIL_CALL_IMPL(impl_reset(), m_validationCallbackMessenger)
    }

    inline result CommandGroup::allocate(const command_list_alloc_desc& desc, CommandList** cmdList)
    {
        LLRI_DETAIL_VALIDATION_REQUIRE(cmdList != nullptr, result::ErrorInvalidUsage)

        *cmdList = nullptr;

        LLRI_DETAIL_VALIDATION_REQUIRE(desc.usage <= command_list_usage::MaxEnum, result::ErrorInvalidUsage)

        LLRI_DETAIL_VALIDATION_REQUIRE(detail::hasSingleBit(desc.nodeMask), result::ErrorInvalidNodeMask)
        LLRI_DETAIL_VALIDATION_REQUIRE(desc.nodeMask < (1 << m_device->m_adapter->queryNodeCount()), result::ErrorInvalidNodeMask)

        LLRI_DETAIL_CALL_IMPL(impl_allocate(desc, cmdList), m_validationCallbackMessenger)
    }

    inline result CommandGroup::allocate(const command_list_alloc_desc& desc, uint8_t count, std::vector<CommandList*>* cmdLists)
    {
        LLRI_DETAIL_VALIDATION_REQUIRE(cmdLists != nullptr, result::ErrorInvalidUsage)

        cmdLists->clear();

        LLRI_DETAIL_VALIDATION_REQUIRE(desc.usage <= command_list_usage::MaxEnum, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE(count > 0, result::ErrorInvalidUsage)

        LLRI_DETAIL_CALL_IMPL(impl_allocate(desc, count, cmdLists), m_validationCallbackMessenger)
    }

    inline result CommandGroup::free(CommandList* cmdList)
    {
        LLRI_DETAIL_VALIDATION_REQUIRE(cmdList != nullptr, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE(detail::contains(m_cmdLists, cmdList), result::ErrorInvalidUsage)

        LLRI_DETAIL_VALIDATION_REQUIRE(cmdList->queryState() != command_list_state::Recording, result::ErrorInvalidState)

        LLRI_DETAIL_CALL_IMPL(impl_free(cmdList), m_validationCallbackMessenger)
    }

    inline result CommandGroup::free(uint8_t numCommandLists, CommandList** cmdLists)
    {
        LLRI_DETAIL_VALIDATION_REQUIRE(cmdLists != nullptr, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE(numCommandLists > 0, result::ErrorInvalidUsage)

#ifdef LLRI_DETAIL_ENABLE_VALIDATION
        for (size_t i = 0; i < numCommandLists; i++)
        {
            LLRI_DETAIL_VALIDATION_REQUIRE_ITER(cmdLists[i] != nullptr, i, result::ErrorInvalidUsage)
            LLRI_DETAIL_VALIDATION_REQUIRE_ITER(detail::contains(m_cmdLists, cmdLists[i]), i, result::ErrorInvalidUsage)
            LLRI_DETAIL_VALIDATION_REQUIRE_ITER(cmdLists[i]->queryState() != command_list_state::Recording, i, result::ErrorInvalidState)
        }
#endif

        LLRI_DETAIL_CALL_IMPL(impl_free(numCommandLists, cmdLists), m_validationCallbackMessenger)
    }
}
