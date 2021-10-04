/**
 * @file device.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-vk/utils.hpp>

namespace LLRI_NAMESPACE
{
    result Device::impl_createCommandGroup(const command_group_desc& desc, CommandGroup** cmdGroup)
    {
        auto* output = new CommandGroup();
        output->m_device = this;
        output->m_deviceFunctionTable = m_functionTable;
        output->m_validationCallbackMessenger = m_validationCallbackMessenger;
        output->m_maxCount = desc.count;
        output->m_type = desc.type;

        auto families = internal::findQueueFamilies(static_cast<VkPhysicalDevice>(m_adapter->m_ptr));

        VkCommandPoolCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.pNext = nullptr;
        info.queueFamilyIndex = families[desc.type];
        info.flags = {};

        VkCommandPool pool;
        const auto r = static_cast<VolkDeviceTable*>(m_functionTable)->
            vkCreateCommandPool(static_cast<VkDevice>(m_ptr), &info, nullptr, &pool);
        if (r != VK_SUCCESS)
        {
            destroyCommandGroup(output);
            return internal::mapVkResult(r);
        }

        output->m_ptr = pool;
        *cmdGroup = output;
        return result::Success;
    }

    void Device::impl_destroyCommandGroup(CommandGroup* cmdGroup)
    {
        if (!cmdGroup)
            return;

        if (cmdGroup->m_ptr)
        {
            static_cast<VolkDeviceTable*>(m_functionTable)->
                vkDestroyCommandPool(static_cast<VkDevice>(m_ptr), static_cast<VkCommandPool>(cmdGroup->m_ptr), nullptr);
        }

        delete cmdGroup;
    }

    result Device::impl_createFence(fence_flags flags, Fence** fence)
    {
        const bool signaled = (flags & fence_flag_bits::Signaled) == fence_flag_bits::Signaled;

        VkFenceCreateFlags vkFlags = 0;
        if (signaled)
            vkFlags |= VK_FENCE_CREATE_SIGNALED_BIT;

        VkFenceCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = vkFlags;

        VkFence vkFence;
        const auto r = static_cast<VolkDeviceTable*>(m_functionTable)->
            vkCreateFence(static_cast<VkDevice>(m_ptr), &info, nullptr, &vkFence);
        if (r != VK_SUCCESS)
            return internal::mapVkResult(r);

        auto* output = new Fence();
        output->m_ptr = vkFence;
        output->m_signaled = signaled;

        *fence = output;
        return result::Success;
    }

    void Device::impl_destroyFence(Fence* fence)
    {
        if (fence->m_ptr)
        {
             static_cast<VolkDeviceTable*>(m_functionTable)->
                vkDestroyFence(static_cast<VkDevice>(m_ptr), static_cast<VkFence>(fence->m_ptr), nullptr);
        }

        delete fence;
    }

    result Device::impl_waitFences(uint32_t numFences, Fence** fences, uint64_t timeout)
    {
        uint64_t vkTimeout = timeout;
        if (timeout != LLRI_TIMEOUT_MAX)
            vkTimeout *= 1000000u; // milliseconds to nanoseconds

        std::vector<VkFence> vkFences(numFences);
        for (size_t i = 0; i < numFences; i++)
            vkFences[i] = static_cast<VkFence>(fences[i]->m_ptr);

        const VkResult r = static_cast<VolkDeviceTable*>(m_functionTable)->
            vkWaitForFences(static_cast<VkDevice>(m_ptr), numFences, vkFences.data(), true, vkTimeout);

        if (r == VK_SUCCESS)
        {
            static_cast<VolkDeviceTable*>(m_functionTable)->
                vkResetFences(static_cast<VkDevice>(m_ptr), numFences, vkFences.data());
        }

        return internal::mapVkResult(r);
    }

    result Device::impl_createSemaphore(Semaphore** semaphore)
    {
        VkSemaphoreCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = {};

        VkSemaphore vkSemaphore;
        const VkResult r = static_cast<VolkDeviceTable*>(m_functionTable)->
            vkCreateSemaphore(static_cast<VkDevice>(m_ptr), &info, nullptr, &vkSemaphore);
        if (r != VK_SUCCESS)
            return internal::mapVkResult(r);

        auto* output = new Semaphore();
        output->m_ptr = vkSemaphore;

        *semaphore = output;
        return result::Success;
    }

    void Device::impl_destroySemaphore(Semaphore* semaphore)
    {
        if (semaphore->m_ptr)
        {
             static_cast<VolkDeviceTable*>(m_functionTable)->
                vkDestroySemaphore(static_cast<VkDevice>(m_ptr), static_cast<VkSemaphore>(semaphore->m_ptr), nullptr);
        }

        delete semaphore;
    }

    result Device::impl_createResource(const resource_desc& desc, Resource** resource)
    {
        const bool isTexture = desc.type != resource_type::Buffer && desc.type != resource_type::MemoryOnly;

        // get all valid queue families
        const auto& families = internal::findQueueFamilies(static_cast<VkPhysicalDevice>(m_adapter->m_ptr));
        std::vector<uint32_t> familyIndices;
        for (const auto& [key, family] : families)
        {
            if (family != UINT_MAX)
                familyIndices.push_back(family);
        }

        // get internal state
        auto internalState = internal::mapResourceState(desc.initialState);

        // get memory flags
        const auto memFlags = internal::mapMemoryType(desc.memoryType);

        uint64_t dataSize = 0;
        uint32_t memoryTypeIndex = 0;

        VkImage image = VK_NULL_HANDLE;
        VkBuffer buffer = VK_NULL_HANDLE;
        if (isTexture)
        {
            uint32_t depth = desc.type == resource_type::Texture3D ? desc.depthOrArrayLayers : 1;
            uint32_t arrayLayers = desc.type == resource_type::Texture3D ? 1 : desc.depthOrArrayLayers;

            VkImageCreateInfo imageCreate;
            imageCreate.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageCreate.pNext = nullptr;
            imageCreate.flags = 0;
            imageCreate.imageType = internal::mapTextureType(desc.type);
            imageCreate.format = internal::mapTextureFormat(desc.format);
            imageCreate.extent = VkExtent3D{ desc.width, desc.height, depth };
            imageCreate.mipLevels = desc.mipLevels;
            imageCreate.arrayLayers = arrayLayers;
            imageCreate.samples = (VkSampleCountFlagBits)desc.sampleCount;
            imageCreate.tiling = internal::mapTextureTiling(desc.tiling);
            imageCreate.usage = internal::mapTextureUsage(desc.usage);
            imageCreate.sharingMode = familyIndices.size() > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
            imageCreate.queueFamilyIndexCount = familyIndices.size();
            imageCreate.pQueueFamilyIndices = familyIndices.data();
            imageCreate.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            internalState = imageCreate.initialLayout;

            auto r = static_cast<VolkDeviceTable*>(m_functionTable)->vkCreateImage(static_cast<VkDevice>(m_ptr), &imageCreate, nullptr, &image);
            if (r != VK_SUCCESS)
                return internal::mapVkResult(r);

            VkMemoryRequirements reqs;
            static_cast<VolkDeviceTable*>(m_functionTable)->vkGetImageMemoryRequirements(static_cast<VkDevice>(m_ptr), image, &reqs);
            dataSize = reqs.size;
            memoryTypeIndex = internal::findMemoryTypeIndex(static_cast<VkPhysicalDevice>(m_adapter->m_ptr), reqs.memoryTypeBits, memFlags);
        }
        else
        {
            VkBufferCreateInfo bufferCreate;
            bufferCreate.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferCreate.pNext = nullptr;
            bufferCreate.flags = 0;
            bufferCreate.size = desc.width;
            bufferCreate.usage = internal::mapBufferUsage(desc.usage);
            bufferCreate.sharingMode = familyIndices.size() > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
            bufferCreate.queueFamilyIndexCount = familyIndices.size();
            bufferCreate.pQueueFamilyIndices = familyIndices.data();

            auto r = static_cast<VolkDeviceTable*>(m_functionTable)->vkCreateBuffer(static_cast<VkDevice>(m_ptr), &bufferCreate, nullptr, &buffer);
            if (r != VK_SUCCESS)
                return internal::mapVkResult(r);

            VkMemoryRequirements reqs;
            static_cast<VolkDeviceTable*>(m_functionTable)->vkGetBufferMemoryRequirements(static_cast<VkDevice>(m_ptr), buffer, &reqs);
            dataSize = reqs.size;
            memoryTypeIndex = internal::findMemoryTypeIndex(static_cast<VkPhysicalDevice>(m_adapter->m_ptr), reqs.memoryTypeBits, memFlags);
        }

        VkMemoryAllocateFlagsInfoKHR flagsInfo;
        flagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
        flagsInfo.pNext = nullptr;
        flagsInfo.deviceMask = desc.visibleNodeMask;
        flagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_MASK_BIT;

        VkMemoryAllocateInfo allocInfo;
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = m_adapter->queryNodeCount() > 1 ? &flagsInfo : nullptr;
        allocInfo.allocationSize = dataSize;
        allocInfo.memoryTypeIndex = memoryTypeIndex;
        
        VkDeviceMemory memory;
        auto r = static_cast<VolkDeviceTable*>(m_functionTable)->vkAllocateMemory(static_cast<VkDevice>(m_ptr), &allocInfo, nullptr, &memory);
        if (r != VK_SUCCESS)
        {
            if (isTexture)
                static_cast<VolkDeviceTable*>(m_functionTable)->vkDestroyImage(static_cast<VkDevice>(m_ptr), image, nullptr);
            else
                static_cast<VolkDeviceTable*>(m_functionTable)->vkDestroyBuffer(static_cast<VkDevice>(m_ptr), buffer, nullptr);
            return internal::mapVkResult(r);
        }

        if (isTexture)
            r = static_cast<VolkDeviceTable*>(m_functionTable)->vkBindImageMemory(static_cast<VkDevice>(m_ptr), image, memory, 0);
        else
            r = static_cast<VolkDeviceTable*>(m_functionTable)->vkBindBufferMemory(static_cast<VkDevice>(m_ptr), buffer, memory, 0);

        if (r != VK_SUCCESS)
        {
            if (isTexture)
                static_cast<VolkDeviceTable*>(m_functionTable)->vkDestroyImage(static_cast<VkDevice>(m_ptr), image, nullptr);
            else
                static_cast<VolkDeviceTable*>(m_functionTable)->vkDestroyBuffer(static_cast<VkDevice>(m_ptr), buffer, nullptr);

            static_cast<VolkDeviceTable*>(m_functionTable)->vkFreeMemory(static_cast<VkDevice>(m_ptr), memory, nullptr);

            return internal::mapVkResult(r);
        }

        auto* output = new Resource();
        output->m_type = desc.type;
        output->m_resource = isTexture ? static_cast<void*>(image) : static_cast<void*>(buffer);
        output->m_memory = memory;

        output->m_state = desc.initialState;
        output->m_implementationState = internalState; // only used for images

        *resource = output;
        return result::Success;
    }

    void Device::impl_destroyResource(Resource* resource)
    {
        bool isTexture = resource->m_type != resource_type::Buffer && resource->m_type != resource_type::MemoryOnly;

        if (isTexture)
            static_cast<VolkDeviceTable*>(m_functionTable)->vkDestroyImage(static_cast<VkDevice>(m_ptr), static_cast<VkImage>(resource->m_resource), nullptr);
        else
            static_cast<VolkDeviceTable*>(m_functionTable)->vkDestroyBuffer(static_cast<VkDevice>(m_ptr), static_cast<VkBuffer>(resource->m_resource), nullptr);
        
        static_cast<VolkDeviceTable*>(m_functionTable)->vkFreeMemory(static_cast<VkDevice>(m_ptr), static_cast<VkDeviceMemory>(resource->m_memory), nullptr);
    }
}
