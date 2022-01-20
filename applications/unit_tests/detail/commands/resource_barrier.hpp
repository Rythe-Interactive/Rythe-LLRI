/**
 * @file resource_barrier.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <helpers.hpp>
#include <doctest/doctest.h>

inline void testCommandListResourceBarrier(llri::Device* device, llri::CommandGroup* group, llri::CommandList* list)
{
    REQUIRE_EQ(group->reset(), llri::result::Success);
    
    llri::resource_barrier dummyBarrier = {};
    
    SUBCASE("Function parameter requirements")
    {
        // command list isn't recording
        CHECK_EQ(list->resourceBarrier(llri::resource_barrier::read_write(nullptr)), llri::result::ErrorInvalidState);
        
        REQUIRE_EQ(group->reset(), llri::result::Success);
        const llri::command_list_begin_desc beginDesc {};
        REQUIRE_EQ(list->record(beginDesc, [=](llri::CommandList* cmd)
        {
            // numBarriers == 0
            CHECK_EQ(list->resourceBarrier(0, &dummyBarrier), llri::result::ErrorInvalidUsage);
                     
            // barriers == nullptr
            CHECK_EQ(list->resourceBarrier(1, nullptr), llri::result::ErrorInvalidUsage);
            
            // barriers[0].type is invalid
            llri::resource_barrier invalidType { static_cast<llri::resource_barrier_type>(UINT_MAX) };
            CHECK_EQ(list->resourceBarrier(1, &invalidType), llri::result::ErrorInvalidUsage);
            
            // barriers[0].rw.resource is nullptr
            llri::resource_barrier rwResourceNull = llri::resource_barrier::read_write(nullptr);
            CHECK_EQ(list->resourceBarrier(1, &rwResourceNull), llri::result::ErrorInvalidUsage);
            
            // barriers[0].trans.resource is nullptr
            llri::resource_barrier transResourceNull = llri::resource_barrier::transition(nullptr, llri::resource_state::TransferDst, llri::resource_state::General);
            CHECK_EQ(list->resourceBarrier(1, &transResourceNull), llri::result::ErrorInvalidUsage);
            
        }, list), llri::result::Success);
    }
    
    llri::resource_desc textureDesc;
    textureDesc.createNodeMask = 0;
    textureDesc.visibleNodeMask = 0;
    textureDesc.type = llri::resource_type::Texture2D;
    textureDesc.usage = llri::resource_usage_flag_bits::TransferDst;
    textureDesc.memoryType = llri::memory_type::Local;
    textureDesc.initialState = llri::resource_state::TransferDst;
    textureDesc.width = 1028;
    textureDesc.height = 1;
    textureDesc.depthOrArrayLayers = 1;
    textureDesc.mipLevels = 1;
    textureDesc.sampleCount = llri::sample_count::Count1;
    textureDesc.textureFormat = llri::format::RGBA8UNorm;
    
    llri::resource_desc bufferDesc = llri::resource_desc::buffer(llri::resource_usage_flag_bits::TransferDst, llri::memory_type::Local, llri::resource_state::General, 1024);
    
	std::vector<llri::Resource*> resources;
	llri::Resource** current;
	
    REQUIRE_EQ(group->reset(), llri::result::Success);
    REQUIRE_EQ(list->begin({}), llri::result::Success);
    
    SUBCASE("resource_barrier_type::ReadWrite")
    {
        SUBCASE("[Correct usage] resource is in ReadWrite state")
        {
            textureDesc.usage |= llri::resource_usage_flag_bits::ShaderWrite;
            textureDesc.initialState = llri::resource_state::ShaderReadWrite;
			
			current = &resources.emplace_back(nullptr);
            REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
            CHECK_EQ(list->resourceBarrier(llri::resource_barrier::read_write(*current)), llri::result::Success);
            
            bufferDesc.usage |= llri::resource_usage_flag_bits::ShaderWrite;
            bufferDesc.initialState = llri::resource_state::ShaderReadWrite;
			
			current = &resources.emplace_back(nullptr);
            REQUIRE_EQ(device->createResource(bufferDesc, current), llri::result::Success);
            CHECK_EQ(list->resourceBarrier(llri::resource_barrier::read_write(*current)), llri::result::Success);
        }
    }
    
    SUBCASE("resource_barrier_type::Transition")
    {
		SUBCASE("[Incorrect usage] barrier.trans.oldState is the same as barrier.trans.newState")
		{
			current = &resources.emplace_back(nullptr);
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::TransferDst)), llri::result::ErrorInvalidUsage);
			
			current = &resources.emplace_back(nullptr);
			REQUIRE_EQ(device->createResource(bufferDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::TransferDst)), llri::result::ErrorInvalidUsage);
		}
		
        SUBCASE("[Incorrect usage] barrier.trans.newState is more than resource_state::MaxEnum")
        {
			current = &resources.emplace_back(nullptr);
            REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
            CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, static_cast<llri::resource_state>(UINT_MAX))), llri::result::ErrorInvalidUsage);
			
			current = &resources.emplace_back(nullptr);
            REQUIRE_EQ(device->createResource(bufferDesc, current), llri::result::Success);
            CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, static_cast<llri::resource_state>(UINT_MAX))), llri::result::ErrorInvalidUsage);
        }
        
        SUBCASE("[Incorrect usage] the resource doesn't have the necessary resource_usage_flags")
        {
            // upload
			current = &resources.emplace_back(nullptr);
            REQUIRE_EQ(device->createResource(bufferDesc, current), llri::result::Success);
            CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::Upload)), llri::result::ErrorInvalidState);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture1D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::Upload)), llri::result::ErrorInvalidState);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture2D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::Upload)), llri::result::ErrorInvalidState);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture3D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::Upload)), llri::result::ErrorInvalidState);
            
            // colorattachment
			current = &resources.emplace_back(nullptr);
            REQUIRE_EQ(device->createResource(bufferDesc, current), llri::result::Success);
            CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ColorAttachment)), llri::result::ErrorInvalidState);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture1D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ColorAttachment)), llri::result::ErrorInvalidState);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture2D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ColorAttachment)), llri::result::ErrorInvalidState);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture3D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ColorAttachment)), llri::result::ErrorInvalidState);
            
            // depthstencilattachment
			current = &resources.emplace_back(nullptr);
            REQUIRE_EQ(device->createResource(bufferDesc, current), llri::result::Success);
            CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::DepthStencilAttachment)), llri::result::ErrorInvalidState);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture1D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::DepthStencilAttachment)), llri::result::ErrorInvalidState);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture2D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::DepthStencilAttachment)), llri::result::ErrorInvalidState);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture3D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::DepthStencilAttachment)), llri::result::ErrorInvalidState);
			
            // depthstencilattachmentreadonly
			current = &resources.emplace_back(nullptr);
            REQUIRE_EQ(device->createResource(bufferDesc, current), llri::result::Success);
            CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::DepthStencilAttachmentReadOnly)), llri::result::ErrorInvalidState);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture1D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::DepthStencilAttachmentReadOnly)), llri::result::ErrorInvalidState);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture2D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::DepthStencilAttachmentReadOnly)), llri::result::ErrorInvalidState);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture3D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::DepthStencilAttachmentReadOnly)), llri::result::ErrorInvalidState);
			
			// shaderreadonly
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture1D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ShaderReadOnly)), llri::result::ErrorInvalidState);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture2D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ShaderReadOnly)), llri::result::ErrorInvalidState);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture3D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ShaderReadOnly)), llri::result::ErrorInvalidState);
			
			// shaderreadwrite
			current = &resources.emplace_back(nullptr);
			REQUIRE_EQ(device->createResource(bufferDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ShaderReadWrite)), llri::result::ErrorInvalidState);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture1D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ShaderReadWrite)), llri::result::ErrorInvalidState);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture2D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ShaderReadWrite)), llri::result::ErrorInvalidState);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture3D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ShaderReadWrite)), llri::result::ErrorInvalidState);
			
			// transfersrc
			current = &resources.emplace_back(nullptr);
			REQUIRE_EQ(device->createResource(bufferDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::TransferSrc)), llri::result::ErrorInvalidState);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture1D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::TransferSrc)), llri::result::ErrorInvalidState);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture2D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::TransferSrc)), llri::result::ErrorInvalidState);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture3D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::TransferSrc)), llri::result::ErrorInvalidState);
			
			// transfer dst
			current = &resources.emplace_back(nullptr);
			bufferDesc.usage = llri::resource_usage_flag_bits::TransferSrc;
			bufferDesc.initialState = llri::resource_state::TransferSrc;
			REQUIRE_EQ(device->createResource(bufferDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferSrc, llri::resource_state::TransferDst)), llri::result::ErrorInvalidState);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.usage = llri::resource_usage_flag_bits::TransferSrc;
			textureDesc.initialState = llri::resource_state::TransferSrc;
			textureDesc.type = llri::resource_type::Texture1D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferSrc, llri::resource_state::TransferDst)), llri::result::ErrorInvalidState);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.usage = llri::resource_usage_flag_bits::TransferSrc;
			textureDesc.initialState = llri::resource_state::TransferSrc;
			textureDesc.type = llri::resource_type::Texture2D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferSrc, llri::resource_state::TransferDst)), llri::result::ErrorInvalidState);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.usage = llri::resource_usage_flag_bits::TransferSrc;
			textureDesc.initialState = llri::resource_state::TransferSrc;
			textureDesc.type = llri::resource_type::Texture3D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferSrc, llri::resource_state::TransferDst)), llri::result::ErrorInvalidState);
			
			// vertex buffer (texture cant be vertex buffer)
			current = &resources.emplace_back(nullptr);
			textureDesc.usage = llri::resource_usage_flag_bits::TransferSrc;
			textureDesc.initialState = llri::resource_state::TransferSrc;
			textureDesc.type = llri::resource_type::Texture2D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferSrc, llri::resource_state::VertexBuffer)), llri::result::ErrorInvalidState);
			
			// index buffer (texture cant be index buffer)
			current = &resources.emplace_back(nullptr);
			textureDesc.usage = llri::resource_usage_flag_bits::TransferSrc;
			textureDesc.initialState = llri::resource_state::TransferSrc;
			textureDesc.type = llri::resource_type::Texture2D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferSrc, llri::resource_state::IndexBuffer)), llri::result::ErrorInvalidState);
			
			// constant buffer (texture cant be constant buffer)
			current = &resources.emplace_back(nullptr);
			textureDesc.usage = llri::resource_usage_flag_bits::TransferSrc;
			textureDesc.initialState = llri::resource_state::TransferSrc;
			textureDesc.type = llri::resource_type::Texture2D;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferSrc, llri::resource_state::ConstantBuffer)), llri::result::ErrorInvalidState);
        }
        
        SUBCASE("[Correct usage] the resource has the right resource_usage_flags")
        {
            // colorattachment
			// ignoring buffer type here since buffers cant be created with colorattachment
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture1D;
			textureDesc.usage = llri::resource_usage_flag_bits::TransferDst | llri::resource_usage_flag_bits::ColorAttachment;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ColorAttachment)), llri::result::Success);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture2D;
			textureDesc.usage = llri::resource_usage_flag_bits::TransferDst | llri::resource_usage_flag_bits::ColorAttachment;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ColorAttachment)), llri::result::Success);

			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture3D;
			textureDesc.usage = llri::resource_usage_flag_bits::TransferDst | llri::resource_usage_flag_bits::ColorAttachment;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ColorAttachment)), llri::result::Success);
            
            // depthstencilattachment
			// ignoring buffer, texture1d, texture3d because they cant be depth-stencil attachments
			textureDesc.textureFormat = llri::format::D32Float;
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture2D;
			textureDesc.usage = llri::resource_usage_flag_bits::TransferDst | llri::resource_usage_flag_bits::DepthStencilAttachment;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::DepthStencilAttachment)), llri::result::Success);
			
            // depthstencilattachmentreadonly
			// ignoring buffer, texture1d, texture3d because they cant be depth-stencil attachments
			textureDesc.textureFormat = llri::format::D32Float;

			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture2D;
			textureDesc.usage = llri::resource_usage_flag_bits::TransferDst | llri::resource_usage_flag_bits::DepthStencilAttachment;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::DepthStencilAttachmentReadOnly)), llri::result::Success);
			
			// shaderreadonly
			textureDesc.textureFormat = llri::format::RGBA8UNorm;

			current = &resources.emplace_back(nullptr);
			bufferDesc.usage = llri::resource_usage_flag_bits::TransferDst; // buffer shouldnt need sampled
			REQUIRE_EQ(device->createResource(bufferDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ShaderReadOnly)), llri::result::Success);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture1D;
			textureDesc.usage = llri::resource_usage_flag_bits::TransferDst | llri::resource_usage_flag_bits::Sampled;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ShaderReadOnly)), llri::result::Success);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture2D;
			textureDesc.usage = llri::resource_usage_flag_bits::TransferDst | llri::resource_usage_flag_bits::Sampled;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ShaderReadOnly)), llri::result::Success);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture3D;
			textureDesc.usage = llri::resource_usage_flag_bits::TransferDst | llri::resource_usage_flag_bits::Sampled;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ShaderReadOnly)), llri::result::Success);
			
			// shaderreadwrite
			textureDesc.textureFormat = llri::format::RGBA8UNorm;

			current = &resources.emplace_back(nullptr);
			bufferDesc.usage = llri::resource_usage_flag_bits::TransferDst | llri::resource_usage_flag_bits::ShaderWrite;
			REQUIRE_EQ(device->createResource(bufferDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ShaderReadWrite)), llri::result::Success);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture1D;
			textureDesc.usage = llri::resource_usage_flag_bits::TransferDst | llri::resource_usage_flag_bits::ShaderWrite;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ShaderReadWrite)), llri::result::Success);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture2D;
			textureDesc.usage = llri::resource_usage_flag_bits::TransferDst | llri::resource_usage_flag_bits::ShaderWrite;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ShaderReadWrite)), llri::result::Success);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture3D;
			textureDesc.usage = llri::resource_usage_flag_bits::TransferDst | llri::resource_usage_flag_bits::ShaderWrite;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ShaderReadWrite)), llri::result::Success);
			
			// transfer src
			current = &resources.emplace_back(nullptr);
			bufferDesc.usage = llri::resource_usage_flag_bits::TransferDst | llri::resource_usage_flag_bits::TransferSrc;
			REQUIRE_EQ(device->createResource(bufferDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::TransferSrc)), llri::result::Success);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture1D;
			textureDesc.usage = llri::resource_usage_flag_bits::TransferDst | llri::resource_usage_flag_bits::TransferSrc;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::TransferSrc)), llri::result::Success);
			
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture2D;
			textureDesc.usage = llri::resource_usage_flag_bits::TransferDst | llri::resource_usage_flag_bits::TransferSrc;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::TransferSrc)), llri::result::Success);

			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture3D;
			textureDesc.usage = llri::resource_usage_flag_bits::TransferDst | llri::resource_usage_flag_bits::TransferSrc;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::TransferSrc)), llri::result::Success);

			// transfer dst
			current = &resources.emplace_back(nullptr);
			bufferDesc.initialState = llri::resource_state::TransferSrc;
			bufferDesc.usage = llri::resource_usage_flag_bits::TransferDst | llri::resource_usage_flag_bits::TransferSrc;
			REQUIRE_EQ(device->createResource(bufferDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferSrc, llri::resource_state::TransferDst)), llri::result::Success);

			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture1D;
			textureDesc.initialState = llri::resource_state::TransferSrc;
			textureDesc.usage = llri::resource_usage_flag_bits::TransferDst | llri::resource_usage_flag_bits::TransferSrc;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferSrc, llri::resource_state::TransferDst)), llri::result::Success);

			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture2D;
			textureDesc.initialState = llri::resource_state::TransferSrc;
			textureDesc.usage = llri::resource_usage_flag_bits::TransferDst | llri::resource_usage_flag_bits::TransferSrc;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferSrc, llri::resource_state::TransferDst)), llri::result::Success);

			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture3D;
			textureDesc.initialState = llri::resource_state::TransferSrc;
			textureDesc.usage = llri::resource_usage_flag_bits::TransferDst | llri::resource_usage_flag_bits::TransferSrc;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferSrc, llri::resource_state::TransferDst)), llri::result::Success);

			// vertexbuffer
			current = &resources.emplace_back(nullptr);
			bufferDesc.initialState = llri::resource_state::TransferDst;
			bufferDesc.usage = llri::resource_usage_flag_bits::TransferDst;
			REQUIRE_EQ(device->createResource(bufferDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::VertexBuffer)), llri::result::Success);

			// indexbuffer
			current = &resources.emplace_back(nullptr);
			bufferDesc.initialState = llri::resource_state::TransferDst;
			bufferDesc.usage = llri::resource_usage_flag_bits::TransferDst;
			REQUIRE_EQ(device->createResource(bufferDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::IndexBuffer)), llri::result::Success);

			// constantbuffer
			current = &resources.emplace_back(nullptr);
			bufferDesc.initialState = llri::resource_state::TransferDst;
			bufferDesc.usage = llri::resource_usage_flag_bits::TransferDst;
			REQUIRE_EQ(device->createResource(bufferDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ConstantBuffer)), llri::result::Success);
        }
		
		SUBCASE("[Correct usage] multiple transitions")
		{
			current = &resources.emplace_back(nullptr);
			bufferDesc.usage = llri::resource_usage_flag_bits::TransferDst | llri::resource_usage_flag_bits::ShaderWrite;
			REQUIRE_EQ(device->createResource(bufferDesc, current), llri::result::Success);
			
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ShaderReadWrite)), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::ShaderReadWrite, llri::resource_state::TransferDst)), llri::result::Success);

			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture1D;
			textureDesc.usage = llri::resource_usage_flag_bits::TransferDst | llri::resource_usage_flag_bits::ShaderWrite;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::TransferDst, llri::resource_state::ShaderReadWrite)), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(llri::resource_barrier::transition(*current, llri::resource_state::ShaderReadWrite, llri::resource_state::TransferDst)), llri::result::Success);
		}
		
		SUBCASE("[Incorrect usage] partial transitions")
		{
			current = &resources.emplace_back(nullptr);
			textureDesc.type = llri::resource_type::Texture2D;
			textureDesc.width = 1024;
			textureDesc.height = 1024;
			textureDesc.mipLevels = 10;
			textureDesc.depthOrArrayLayers = 2;
			textureDesc.initialState = llri::resource_state::TransferSrc;
			textureDesc.usage = llri::resource_usage_flag_bits::TransferSrc | llri::resource_usage_flag_bits::TransferDst;
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			
			// baseMipLevel >= textureDesc.mipLevels
			CHECK_EQ(list->resourceBarrier(
					llri::resource_barrier::transition(
						*current,
						llri::resource_state::TransferSrc, llri::resource_state::TransferDst,
					    llri::texture_subresource_range { textureDesc.mipLevels, 1, 0, 1 }
					)
				), llri::result::ErrorInvalidUsage);
			
			// numMipLevels == 0
			CHECK_EQ(list->resourceBarrier(
					llri::resource_barrier::transition(
						*current,
						llri::resource_state::TransferSrc, llri::resource_state::TransferDst,
					    llri::texture_subresource_range { 0, 0, 0, 1 }
					)
				), llri::result::ErrorInvalidUsage);
			
			// (baseMipLevel + numMipLevels) > textureDesc.mipLevels
			CHECK_EQ(list->resourceBarrier(
					llri::resource_barrier::transition(
						*current,
						llri::resource_state::TransferSrc, llri::resource_state::TransferDst,
						llri::texture_subresource_range { 9, 5, 0, 1 }
					)
				), llri::result::ErrorInvalidUsage);
			
			// baseArrayLayer >= textureDesc.depthOrArrayLayers
			CHECK_EQ(list->resourceBarrier(
					llri::resource_barrier::transition(
						*current,
						llri::resource_state::TransferSrc, llri::resource_state::TransferDst,
						llri::texture_subresource_range { 0, 1, 2, 1 }
					)
				), llri::result::ErrorInvalidUsage);
			
			// numArrayLayers == 0
			CHECK_EQ(list->resourceBarrier(
					llri::resource_barrier::transition(
						*current,
						llri::resource_state::TransferSrc, llri::resource_state::TransferDst,
						llri::texture_subresource_range { 0, 1, 0, 0 }
					)
				), llri::result::ErrorInvalidUsage);
			
			// (baseArrayLayer + numArrayLayers) > textureDesc.depthOrArrayLayers
			CHECK_EQ(list->resourceBarrier(
					llri::resource_barrier::transition(
						*current,
						llri::resource_state::TransferSrc, llri::resource_state::TransferDst,
						llri::texture_subresource_range { 0, 1, 1, 2 }
					)
				), llri::result::ErrorInvalidUsage);
		}
		
		SUBCASE("[Correct usage] partial transitions")
		{
			textureDesc.type = llri::resource_type::Texture2D;
			textureDesc.width = 1024;
			textureDesc.height = 1024;
			textureDesc.mipLevels = 10;
			textureDesc.depthOrArrayLayers = 2;
			textureDesc.initialState = llri::resource_state::TransferSrc;
			textureDesc.usage = llri::resource_usage_flag_bits::TransferSrc | llri::resource_usage_flag_bits::TransferDst;
			
			// one miplevel, one array layer
			current = &resources.emplace_back(nullptr);
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(
					llri::resource_barrier::transition(
						*current,
						llri::resource_state::TransferSrc, llri::resource_state::TransferDst,
						llri::texture_subresource_range { 0, 1, 0, 1 }
					)
				), llri::result::Success);
			
			// multiple miplevels, one array layer
			current = &resources.emplace_back(nullptr);
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(
					llri::resource_barrier::transition(
						*current,
						llri::resource_state::TransferSrc, llri::resource_state::TransferDst,
						llri::texture_subresource_range { 0, 5, 0, 1 }
					)
				), llri::result::Success);
			
			// one miplevel, multiple array layers
			current = &resources.emplace_back(nullptr);
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(
					llri::resource_barrier::transition(
						*current,
						llri::resource_state::TransferSrc, llri::resource_state::TransferDst,
						llri::texture_subresource_range { 0, 1, 0, 2 }
					)
				), llri::result::Success);
			
			// multiple miplevels, multiple array layers
			current = &resources.emplace_back(nullptr);
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(
					llri::resource_barrier::transition(
						*current,
						llri::resource_state::TransferSrc, llri::resource_state::TransferDst,
						llri::texture_subresource_range { 0, 5, 0, 2 }
					)
				), llri::result::Success);
			
			// miplevels at an offset, array levels at an offset
			current = &resources.emplace_back(nullptr);
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			CHECK_EQ(list->resourceBarrier(
					llri::resource_barrier::transition(
						*current,
						llri::resource_state::TransferSrc, llri::resource_state::TransferDst,
						llri::texture_subresource_range { 3, 2, 1, 1 }
					)
				), llri::result::Success);
		}
		
		SUBCASE("[Correct usage] multiple partial transitions")
		{
			textureDesc.type = llri::resource_type::Texture2D;
			textureDesc.width = 1024;
			textureDesc.height = 1024;
			textureDesc.mipLevels = 10;
			textureDesc.depthOrArrayLayers = 2;
			textureDesc.initialState = llri::resource_state::TransferDst;
			textureDesc.usage = llri::resource_usage_flag_bits::TransferDst | llri::resource_usage_flag_bits::Sampled | llri::resource_usage_flag_bits::ShaderWrite;
			
			current = &resources.emplace_back(nullptr);
			REQUIRE_EQ(device->createResource(textureDesc, current), llri::result::Success);
			
			// a common usage may be to transition one resource to shaderwrite
			// and the others to shader read only
			CHECK_EQ(list->resourceBarrier(
					llri::resource_barrier::transition(
						*current,
						llri::resource_state::TransferDst, llri::resource_state::ShaderReadWrite,
						llri::texture_subresource_range { 0, 1, 0, 1 }
					)
				), llri::result::Success);
			
			CHECK_EQ(list->resourceBarrier(
					llri::resource_barrier::transition(
						*current,
						llri::resource_state::TransferDst, llri::resource_state::ShaderReadOnly,
						llri::texture_subresource_range { 1, 9, 0, 1 }
					)
				), llri::result::Success);
		}
    }
    
    CHECK_EQ(list->end(), llri::result::Success);
	
	auto* queue = device->getQueue(helpers::availableQueueType(device->getAdapter()), 0);

	llri::Fence* fence;
	REQUIRE_EQ(device->createFence({}, &fence), llri::result::Success);
	
	const llri::submit_desc submitDesc { 0, 1, &list, 0, nullptr, 0, nullptr, fence };
	CHECK_EQ(queue->submit(submitDesc), llri::result::Success);
	
	queue->waitIdle();
	
	device->destroyFence(fence);
	
	for (auto* resource : resources)
		device->destroyResource(resource);
}
