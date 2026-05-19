#include "PersistanceVk.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


void PersistanceVk::SetUpDebugCallBack()
{
	if (!enablevalidationlayers)
	{
		return;
	}

	VkDebugUtilsMessengerCreateInfoEXT createinfo;
	SetDebugCreateInfoStructVariables(createinfo);

	if (DebugUtilsMessengerEXT::Create(m_instance, &createinfo, nullptr, &debugmessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug messenger");

	}

}

void PersistanceVk::CreateInstance()
{
	if (enablevalidationlayers && !CheckValidationLayers())
	{
		std::cout << "Tried Getting Validation Layers and Failed. \n";
		BREAK;
	}

	//application info struct
	VkApplicationInfo appinfo{};
	appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appinfo.pApplicationName = "Game";
	appinfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
	appinfo.pEngineName = "Persistance";
	appinfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appinfo.apiVersion = VK_API_VERSION_1_0;

	//instance create info struct
	VkInstanceCreateInfo createinfo{};
	createinfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createinfo.pApplicationInfo = &appinfo;
	


	VkDebugUtilsMessengerCreateInfoEXT debugcreateinfo{};

	if (enablevalidationlayers)
	{
		createinfo.enabledLayerCount = static_cast<uint32_t>(m_validationlayers.size());
		createinfo.ppEnabledLayerNames = m_validationlayers.data();

		SetDebugCreateInfoStructVariables(debugcreateinfo);
		createinfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugcreateinfo;


	}
	else
	{
		createinfo.enabledLayerCount = 0;
		createinfo.pNext = nullptr;
	}

	createinfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

	std::vector<const char*> requiredextensions = GetRequiredInstanceExtensions();

	createinfo.enabledExtensionCount = (uint32_t)requiredextensions.size();
	createinfo.ppEnabledExtensionNames = requiredextensions.data();
	


	if (vkCreateInstance(&createinfo, nullptr, &m_instance) != VK_SUCCESS)
	{
		std::cout << "Failed at Creating Instance \n";
		BREAK;

	}
	else
	{
		std::cout << "Created PersistanceVk Instance\n";
		std::cout << "PersistanceVk Name: " << appinfo.pApplicationName << "\nEngine: "<< appinfo.pEngineName<< "\n";
		

	}

}

void PersistanceVk::CreateSwapChain()
{
	SwapChainSupportDetails details = QuerySwapChainSupport(m_physicaldevice);

	VkSurfaceFormatKHR surfaceformat = ChooseSwapSurfaceFormat(details.surfaceformat);
	VkPresentModeKHR surfacepresentmode = ChooseSwapPresentMode(details.presentmode);
	VkExtent2D extent = ChooseSwapExtent(details.surfacecapabilities);

	uint32_t imagecount = details.surfacecapabilities.minImageCount + 1;

	if (details.surfacecapabilities.maxImageCount > 0 && imagecount > details.surfacecapabilities.maxImageCount)
	{
		imagecount = details.surfacecapabilities.maxImageCount;

	}


	VkSwapchainCreateInfoKHR createinfo{};

	createinfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

	createinfo.minImageCount = imagecount;


	createinfo.surface = m_surface;

	createinfo.imageFormat = surfaceformat.format;

	createinfo.imageColorSpace = surfaceformat.colorSpace;

	createinfo.imageExtent = extent;

	createinfo.imageArrayLayers = 1;

	createinfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;


	QueueFamilyIndices indices = FindQueueFamilies(m_physicaldevice);
	
	uint32_t queuefamilyindices[] =
	{
		indices.graphicsfamily, indices.presentfamily
	};

	if (indices.graphicsfamily != indices.presentfamily)
	{
		createinfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createinfo.queueFamilyIndexCount = 2;
		createinfo.pQueueFamilyIndices = queuefamilyindices;


	}
	else
	{
		createinfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createinfo.queueFamilyIndexCount = 0;
		createinfo.pQueueFamilyIndices = nullptr;

	}


	createinfo.preTransform = details.surfacecapabilities.currentTransform;
	
	createinfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	
	createinfo.presentMode = surfacepresentmode;

	createinfo.clipped = VK_TRUE;

	createinfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(m_device, &createinfo, nullptr, &m_swapchain) != VK_SUCCESS)
	{
		throw std::runtime_error("Couldn't create swapchain");

	}

	uint32_t swapchainimagecount = 0;

	vkGetSwapchainImagesKHR(m_device, m_swapchain, &swapchainimagecount, nullptr);

	m_swapchainimages.resize(swapchainimagecount);

	vkGetSwapchainImagesKHR(m_device, m_swapchain, &swapchainimagecount, m_swapchainimages.data());

	m_swapchainimageformat = surfaceformat.format;
	
	m_swapchainextent = extent;








}

void PersistanceVk::CreateImageViews()
{
	m_swapchainimageviews.resize(m_swapchainimages.size());

	for (int i = 0; i < m_swapchainimages.size(); i++)
	{
		m_swapchainimageviews[i] = CreateImageView(m_swapchainimages[i], m_swapchainimageformat);
	}
}

void PersistanceVk::CreateSwapchainFramebuffers(uint32_t renderpasshandle)
{
	m_swapchainframebuffers.resize(m_swapchainimageviews.size());
	//using a handle, this would be replaced by a stored vector of framebuffers being resized according to the amount of framebuffers we want to make.

	for (int i = 0; i < m_swapchainimageviews.size(); i++)
	{

		VkImageView attachments[] = { m_swapchainimageviews[i] }; //separate this into another map

		VkFramebufferCreateInfo framebufferinfo{};
		framebufferinfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferinfo.renderPass = mh_renderpasses.at(renderpasshandle).renderpass;
		framebufferinfo.layers = 1;
		framebufferinfo.attachmentCount = 1;
		framebufferinfo.pAttachments = attachments;
		framebufferinfo.width = m_swapchainextent.width;
		framebufferinfo.height = m_swapchainextent.height;

		if (vkCreateFramebuffer(m_device, &framebufferinfo, nullptr, &m_swapchainframebuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create framebuffer");

		}


	}
}



void PersistanceVk::CreateCommandPools()
{
	
	QueueFamilyIndices indices = FindQueueFamilies(m_physicaldevice);

	if (indices.graphicsfamily != -1)
	{
		VkCommandPoolCreateInfo graphicspoolinfo{};
		graphicspoolinfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		graphicspoolinfo.queueFamilyIndex = indices.graphicsfamily;
		graphicspoolinfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if (vkCreateCommandPool(m_device, &graphicspoolinfo, nullptr, &m_graphicscommandpool) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create the graphics command pool!");

		}

	}

	if (indices.transferfamily != -1)
	{
		VkCommandPoolCreateInfo transferpoolinfo{};
		transferpoolinfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		transferpoolinfo.queueFamilyIndex = indices.transferfamily;
		transferpoolinfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if (vkCreateCommandPool(m_device, &transferpoolinfo, nullptr, &m_transfercommandpool) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create the transfer command pool!");
		}

	}
	
}



DescriptorPoolHandle PersistanceVk::CreateDescriptorPoolHandle()
{
	uint32_t handle = m_descriptorpoolcount++;
	mh_descriptorpools.emplace_back(DescriptorPool());

	return handle;
}

void PersistanceVk::AddDescriptorPoolSize(DescriptorPoolHandle handle, VkDescriptorType type)
{
	VkDescriptorPoolSize size;
	size.descriptorCount = static_cast<uint32_t>(PersistanceLib::MAXFRAMESINFLIGHT);
	size.type = type;

	mh_descriptorpools.at(handle).poolsizes.push_back(size);

}

void PersistanceVk::CreateDescriptorPool(DescriptorPoolHandle handle)
{
	std::vector<VkDescriptorPoolSize>& poolsizes = mh_descriptorpools.at(handle).poolsizes;

	VkDescriptorPoolCreateInfo poolinfo{};
	poolinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolinfo.poolSizeCount = static_cast<uint32_t>(poolsizes.size());
	poolinfo.pPoolSizes = poolsizes.data();
	poolinfo.maxSets = static_cast<uint32_t>(PersistanceLib::MAXFRAMESINFLIGHT);

	if (vkCreateDescriptorPool(m_device, &poolinfo, nullptr, &mh_descriptorpools.at(handle).pool) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create descriptor pool!");

	}


}

void PersistanceVk::CleanDescriptorPools()
{
	for (int i = 0; i < mh_descriptorpools.size(); i++) 
	{
		vkDestroyDescriptorPool(m_device, mh_descriptorpools[i].pool, nullptr);

	}
}

DescriptorSetHandle PersistanceVk::CreateDescriptorSetHandle()
{
	uint32_t handle = m_descriptorsetcount++;
	mh_descriptorsets.emplace_back(DescriptorSet());

	return handle;
}

void PersistanceVk::CreateDescriptorSets(DescriptorSetHandle handle, uint32_t layouthandle, uint32_t poolhandle)
{
	// first we allocate descriptorsets for every possible frame in flight. 
	std::vector<VkDescriptorSetLayout> layouts(static_cast<uint32_t>(PersistanceLib::MAXFRAMESINFLIGHT), mh_descriptorsetlayouts.at(layouthandle).layout);
	VkDescriptorSetAllocateInfo allocateinfo{};
	allocateinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateinfo.descriptorPool = mh_descriptorpools.at(poolhandle).pool;
	allocateinfo.descriptorSetCount = static_cast<uint32_t>(PersistanceLib::MAXFRAMESINFLIGHT);
	allocateinfo.pSetLayouts = layouts.data();

	// this is done by indexing into the unordered map of descriptorsetlayouts with the layouthandle variable

	mh_descriptorsets.at(handle).descriptorsets.resize(PersistanceLib::MAXFRAMESINFLIGHT);

	if (vkAllocateDescriptorSets(m_device, &allocateinfo, mh_descriptorsets.at(handle).descriptorsets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate descriptor set");
	}

	// Here's where things get tricky!
	// This for loop will set the writedescriptorsets for the descriptorsets we're creating. 
	// writedescriptorsets need bufferinfos, imageinfos or texelbufferinfos to work.
	// as of now we're only using bufferinfos and imageinfos.
	for (int i = 0; i < PersistanceLib::MAXFRAMESINFLIGHT; i++) 
	{
		std::vector<WriteDescriptorSet>& set = mh_descriptorsets.at(handle).writedescriptorsets;

		std::vector<VkWriteDescriptorSet> writedescriptors{};
		writedescriptors.resize(set.size());
		std::vector<VkDescriptorBufferInfo> bufferinfos{}; //we create all bufferinfos and imageinfos we need.
		std::vector<VkDescriptorImageInfo> imageinfos{};

		for (int w = 0; w < set.size(); w++) //for all write descriptors in our descriptorset
		{
			writedescriptors[w].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writedescriptors[w].dstSet = mh_descriptorsets.at(handle).descriptorsets[i];
			writedescriptors[w].dstArrayElement = set[w].arrayelement;
			writedescriptors[w].descriptorCount = set[w].descriptorcount;
			writedescriptors[w].descriptorType = set[w].descriptorType;
			writedescriptors[w].dstBinding = set[w].bindingidx;
			if (set[w].bufferinfo.size() > 0) // if we're using a bufferinfo instead of an image info
			{
				bufferinfos.resize(set[w].descriptorcount);
				for (int z = 0; z < bufferinfos.size(); z++) // for all bufferinfos specified in the descriptorCount variable
				{
					bufferinfos[z].buffer = mh_uniformbuffers.at(set[w].bufferinfo[z].uniformbufferhandle).buffers[i]; // we set the values stored in the bufferinfo structs inside writedescriptor struct
					bufferinfos[z].offset = set[w].bufferinfo[z].offset;
					bufferinfos[z].range = set[w].bufferinfo[z].range;
				}
				writedescriptors[w].pBufferInfo = bufferinfos.data(); // we set the pBufferInfo variable to all bufferinfos created
			}
			else if (set[w].imageinfo.size() > 0) // if we're using imageinfos instead of bufferinfos
			{
				imageinfos.resize(set[w].descriptorcount);
				for (int z = 0; z < imageinfos.size(); z++)
				{
					imageinfos[z].imageLayout = set[w].imageinfo[z].imagelayout;
					imageinfos[z].imageView = set[w].imageinfo[z].imageview;
					imageinfos[z].sampler = set[w].imageinfo[z].sampler;
				}
				writedescriptors[w].pImageInfo = imageinfos.data();
			}

			
		}

		vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(writedescriptors.size()), writedescriptors.data(), 0, nullptr);
		

	}
	

}

WriteDescriptorSet* PersistanceVk::CreateWriteDescriptorSet(DescriptorSetHandle handle, uint32_t descriptorcount, uint32_t bindingidx, VkDescriptorType descriptortype, uint32_t* writedescriptorindex)
{
	uint32_t idx = mh_descriptorsets.at(handle).writedescriptorsets.size();
	mh_descriptorsets.at(handle).writedescriptorsets.push_back(WriteDescriptorSet());
	WriteDescriptorSet* set = &mh_descriptorsets.at(handle).writedescriptorsets[idx];
	set->bindingidx = bindingidx;
	set->descriptorcount = descriptorcount;
	set->descriptorType = descriptortype;

	*writedescriptorindex = idx;

	return set;
}

void PersistanceVk::AddDescriptorBufferInfoToWriteDescriptorSet(DescriptorSetHandle handle, uint32_t writedescriptorindex, uint32_t uniformbufferhandle, size_t offset, size_t range)
{

	if (mh_descriptorsets.at(handle).writedescriptorsets[writedescriptorindex].imageinfo.size() > 0)
	{
		throw std::runtime_error("You already have an image info in this writedescriptorset");
	}

	DescriptorBufferInfo info;
	info.offset = offset;
	info.range = range;
	info.uniformbufferhandle = uniformbufferhandle;

	mh_descriptorsets.at(handle).writedescriptorsets[writedescriptorindex].bufferinfo.push_back(info);

}

void PersistanceVk::AddDescriptorImageInfoToWriteDescriptorSet(DescriptorSetHandle handle, uint32_t writedescriptorindex, VkImageLayout imagelayout, TextureHandle texturehandle, TextureSamplerHandle texturesamplerhandle)
{
	if (mh_descriptorsets.at(handle).writedescriptorsets[writedescriptorindex].bufferinfo.size() > 0)
	{
		throw std::runtime_error("You already have a buffer info in this writedescriptorset");
	}

	DescriptorImageInfo info;
	info.imagelayout = imagelayout;
	info.imageview = mh_textures.at(texturehandle).imageview;
	info.sampler = mh_texturesamplers.at(texturesamplerhandle);

	mh_descriptorsets.at(handle).writedescriptorsets[writedescriptorindex].imageinfo.push_back(info);

}

BufferHandle PersistanceVk::CreateCommandBufferHandle()
{
	uint32_t handle = m_commandbuffercount++;
	mh_commandbuffers.emplace_back(std::vector<VkCommandBuffer>());

	return handle;
}

void PersistanceVk::CreateCommandBuffer(BufferHandle handle, VkCommandPool& commandpool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY)
{

	mh_commandbuffers.at(handle).resize(PersistanceLib::MAXFRAMESINFLIGHT);

	VkCommandBufferAllocateInfo cmdbufferinfo{};
	cmdbufferinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdbufferinfo.commandBufferCount = static_cast<uint32_t>(PersistanceLib::MAXFRAMESINFLIGHT);
	cmdbufferinfo.commandPool = commandpool;
	cmdbufferinfo.level = level;

	if (vkAllocateCommandBuffers(m_device, &cmdbufferinfo, mh_commandbuffers.at(handle).data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create command buffer");
	}


}

void PersistanceVk::StartDrawing(BufferHandle commandbufferhandle, RenderPassHandle renderpasshandle)
{
	if (m_drawingstarted)
	{
		std::cout << "Previous drawing not ended properly";
	}
	else 
	{
		m_drawingstarted = true;
	}

	vkWaitForFences(m_device, 1, &f_inflightfence[m_currentframe], VK_TRUE, UINT64_MAX);

	
	VkResult result = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, s_imageavailable[m_currentframe], nullptr, &m_imageindex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapchain(renderpasshandle);
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to aquire swapchain image!");
	}
	if (f_imagesinflight[m_imageindex] != VK_NULL_HANDLE)
	{
		vkWaitForFences(m_device, 1, &f_imagesinflight[m_imageindex], true, UINT64_MAX);
	}
	f_imagesinflight[m_imageindex] = f_inflightfence[m_currentframe];
	vkResetFences(m_device, 1, &f_inflightfence[m_currentframe]);

	vkResetCommandBuffer(mh_commandbuffers.at(commandbufferhandle)[m_currentframe], 0);

}

void PersistanceVk::RecordCommandBuffer(VkCommandBuffer& commandbuffer, const uint32_t& swapchainimageindex, const GraphicsPipelineHandle graphicspipelinehandle, const BufferHandle vertexbufferhandle, const BufferHandle indexbufferhandle, const DescriptorSetHandle descriptorsethandle, const RenderPassHandle renderpasshandle)
{
	VkCommandBufferBeginInfo cmdbufferbegininfo{};
	cmdbufferbegininfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdbufferbegininfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	cmdbufferbegininfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(commandbuffer, &cmdbufferbegininfo) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to begin command buffer!");

	}

	VkRenderPassBeginInfo renderpassbegininfo{};
	renderpassbegininfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderpassbegininfo.framebuffer = m_swapchainframebuffers[swapchainimageindex];
	renderpassbegininfo.renderPass = mh_renderpasses.at(renderpasshandle).renderpass;

	renderpassbegininfo.renderArea.offset = { 0,0 };
	renderpassbegininfo.renderArea.extent = m_swapchainextent;

	VkClearValue clearcolor = { {{0.f, 0.f, 0.f, 1.0f}} };
	renderpassbegininfo.clearValueCount = 1;
	renderpassbegininfo.pClearValues = &clearcolor;


	vkCmdBeginRenderPass(commandbuffer, &renderpassbegininfo, VK_SUBPASS_CONTENTS_INLINE);


	VkViewport viewport{};
	viewport.x = 0.f;
	viewport.y = 0.f;
	viewport.minDepth = 0.f;
	viewport.maxDepth = 1.f;
	viewport.width = (float)m_swapchainextent.width;
	viewport.height = (float)m_swapchainextent.height;
	vkCmdSetViewport(commandbuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0,0 };
	scissor.extent = m_swapchainextent;
	vkCmdSetScissor(commandbuffer, 0, 1, &scissor);


	vkCmdBindPipeline(commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mh_graphicspipelines.at(graphicspipelinehandle).pipeline);

	VkBuffer buffers[] = { mh_vertexbuffers.at(vertexbufferhandle).buffer };//{m_vertexbuffer};
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandbuffer, 0, 1, buffers, offsets);
	
	vkCmdBindIndexBuffer(commandbuffer, mh_indexbuffers.at(indexbufferhandle).buffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdBindDescriptorSets(commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mh_graphicspipelines.at(graphicspipelinehandle).layout, 0, 1, &mh_descriptorsets.at(descriptorsethandle).descriptorsets[m_currentframe], 0, nullptr);

	vkCmdDrawIndexed(commandbuffer, static_cast<uint32_t> (mh_indexbuffers.at(indexbufferhandle).size / sizeof(uint32_t)), 1, 0, 0, 0);

	vkCmdEndRenderPass(commandbuffer);

	if (vkEndCommandBuffer(commandbuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to record command buffer");

	}
}

void PersistanceVk::BeginCommandBuffer(BufferHandle commandbufferhandle, uint32_t currentframe, VkCommandBufferUsageFlags flags)
{
	VkCommandBufferBeginInfo cmdbufferbegininfo{};
	cmdbufferbegininfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdbufferbegininfo.flags = flags;
	cmdbufferbegininfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(mh_commandbuffers.at(commandbufferhandle).at(currentframe), &cmdbufferbegininfo) != VK_SUCCESS)
	{
		std::cout << "Couldnt begin command buffer \n";
		BREAK(0);

	}
}

void PersistanceVk::BeginRenderPass(BufferHandle commandbufferhandle, uint32_t commandbufferframe, RenderPassHandle renderpasshandle, VkClearValue clearcolor, VkRect2D offset, VkExtent2D extent)
{
	VkRenderPassBeginInfo renderpassbegininfo{};
	renderpassbegininfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderpassbegininfo.framebuffer = m_swapchainframebuffers[m_imageindex];
	renderpassbegininfo.renderPass = mh_renderpasses.at(renderpasshandle).renderpass;

	renderpassbegininfo.renderArea.offset = { 0,0 };
	renderpassbegininfo.renderArea.extent = m_swapchainextent;

	renderpassbegininfo.clearValueCount = 1;
	renderpassbegininfo.pClearValues = &clearcolor;


	vkCmdBeginRenderPass(mh_commandbuffers.at(commandbufferhandle).at(commandbufferframe), &renderpassbegininfo, VK_SUBPASS_CONTENTS_INLINE);
}

void PersistanceVk::BindGraphicsPipeline(BufferHandle commandbufferhandle, uint32_t commandbufferframe, VkPipelineBindPoint bindingpoint, GraphicsPipelineHandle handle)
{
	vkCmdBindPipeline(mh_commandbuffers.at(commandbufferhandle).at(commandbufferframe), bindingpoint, mh_graphicspipelines.at(handle).pipeline);
}

void PersistanceVk::SetViewport(BufferHandle commandbufferhandle, uint32_t commandbufferframe, float xpos, float ypos, float mindepth, float maxdepth, VkExtent2D extent)
{
	VkViewport viewport{};
	viewport.x = xpos;
	viewport.y = ypos;
	viewport.minDepth = mindepth;
	viewport.maxDepth = maxdepth;
	viewport.width = (float)extent.width;
	viewport.height = (float)extent.height;
	vkCmdSetViewport(mh_commandbuffers.at(commandbufferhandle).at(commandbufferframe), 0, 1, &viewport);
}

void PersistanceVk::SetScissors(BufferHandle commandbufferhandle, uint32_t commandbufferframe, VkOffset2D offset, VkExtent2D extent)
{
	VkRect2D scissor{};
	scissor.offset = offset;
	scissor.extent = extent;
	vkCmdSetScissor(mh_commandbuffers.at(commandbufferhandle).at(commandbufferframe), 0, 1, &scissor);
}

void PersistanceVk::Draw(BufferHandle commandbufferhandle, uint32_t commandbufferframe, Drawable drawsettings)
{
	VkCommandBuffer& commandbuffer = mh_commandbuffers.at(commandbufferhandle).at(commandbufferframe);

	BindGraphicsPipeline(commandbufferhandle, commandbufferframe, drawsettings.GetGraphicsPipelineBindingPoint(), drawsettings.GetGraphicsPipelineHandle());

	size_t buffercount = drawsettings.GetVertexBufferHandles().size();
	size_t offsetcount = drawsettings.GetVertexBufferOffsets().size();
	std::vector<VkBuffer> buffers;
	buffers.reserve(buffercount);

	size_t vertexcount = 0;

	for (size_t i = 0; i < buffercount; i++) {
		buffers.emplace_back(mh_vertexbuffers.at(drawsettings.GetVertexBufferHandles()[i]).buffer);
		vertexcount += mh_vertexbuffers.at(drawsettings.GetVertexBufferHandles()[i]).size;
	}

	vertexcount /= sizeof(uint32_t);

	vkCmdBindVertexBuffers(commandbuffer, 0, 1, buffers.data(), drawsettings.GetVertexBufferOffsets().data());

	vkCmdBindDescriptorSets(commandbuffer, drawsettings.GetGraphicsPipelineBindingPoint(), mh_graphicspipelines.at(drawsettings.GetGraphicsPipelineHandle()).layout, 0, 1, &mh_descriptorsets.at(drawsettings.GetDescriptorSetHandle()).descriptorsets[commandbufferframe], 0, nullptr);

	vkCmdDraw(mh_commandbuffers.at(commandbufferhandle).at(commandbufferframe), vertexcount, 1, 0, 0);
}

void PersistanceVk::DrawIndexed(BufferHandle commandbufferhandle, uint32_t commandbufferframe, Drawable drawsettings)
{
	VkCommandBuffer& commandbuffer = mh_commandbuffers.at(commandbufferhandle).at(commandbufferframe);

	BindGraphicsPipeline(commandbufferhandle, commandbufferframe, drawsettings.GetGraphicsPipelineBindingPoint(), drawsettings.GetGraphicsPipelineHandle());

	size_t buffercount = drawsettings.GetVertexBufferHandles().size();
	size_t offsetcount = drawsettings.GetVertexBufferOffsets().size();
	std::vector<VkBuffer> buffers;
	buffers.reserve(buffercount);

	for (size_t i = 0; i < buffercount; i++) {
		buffers.emplace_back(mh_vertexbuffers.at(drawsettings.GetVertexBufferHandles()[i]).buffer);
		
	}

	vkCmdBindVertexBuffers(commandbuffer, 0, 1, buffers.data(), drawsettings.GetVertexBufferOffsets().data());

	vkCmdBindIndexBuffer(commandbuffer, mh_indexbuffers.at(drawsettings.GetIndexBufferHandle()).buffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdBindDescriptorSets(commandbuffer, drawsettings.GetGraphicsPipelineBindingPoint(), mh_graphicspipelines.at(drawsettings.GetGraphicsPipelineHandle()).layout, 0, 1, &mh_descriptorsets.at(drawsettings.GetDescriptorSetHandle()).descriptorsets[commandbufferframe], 0, nullptr);

	vkCmdDrawIndexed(commandbuffer, static_cast<uint32_t> (mh_indexbuffers.at(drawsettings.GetIndexBufferHandle()).size/sizeof(uint32_t)), 1, 0, 0, 0);

}

void PersistanceVk::EndRenderPass(BufferHandle commandbufferhandle, uint32_t commandbufferframe)
{
	vkCmdEndRenderPass(mh_commandbuffers.at(commandbufferhandle).at(commandbufferframe));
}

void PersistanceVk::EndCommandBuffer(BufferHandle commandbufferhandle, uint32_t commandbufferframe)
{
	if (vkEndCommandBuffer(mh_commandbuffers.at(commandbufferhandle).at(commandbufferframe)) != VK_SUCCESS) 
	{
		BREAK(0);
	}
}

void PersistanceVk::EndAndPresentDrawing(BufferHandle commandbufferhandle, RenderPassHandle renderpasshandle)
{
	VkSemaphore waitsemaphores[] = { s_imageavailable[m_currentframe] };
	VkSemaphore signalsemaphores[] = { s_renderfinished[m_currentframe] };
	VkPipelineStageFlags waitstages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submitinfo{};


	submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitinfo.commandBufferCount = 1;
	submitinfo.pCommandBuffers = &mh_commandbuffers.at(commandbufferhandle)[m_currentframe];
	submitinfo.waitSemaphoreCount = 1;
	submitinfo.pWaitSemaphores = waitsemaphores;
	submitinfo.pWaitDstStageMask = waitstages;
	submitinfo.signalSemaphoreCount = 1;
	submitinfo.pSignalSemaphores = signalsemaphores;



	if (vkQueueSubmit(m_graphicsqueue, 1, &submitinfo, f_inflightfence[m_currentframe]) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to submit graphics queue!");

	}

	VkSwapchainKHR swapchains[] = { m_swapchain };

	VkPresentInfoKHR presentinfo{};
	presentinfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentinfo.waitSemaphoreCount = 1;
	presentinfo.pWaitSemaphores = signalsemaphores;

	presentinfo.swapchainCount = 1;
	presentinfo.pSwapchains = swapchains;

	presentinfo.pImageIndices = &m_imageindex;

	presentinfo.pResults = nullptr;

	VkResult result = vkQueuePresentKHR(m_presentqueue, &presentinfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result != VK_SUBOPTIMAL_KHR || m_windowresized)
	{
		RecreateSwapchain(renderpasshandle);
		m_windowresized = false;

	}
	else if (result != VK_SUCCESS)
	{
		std::cout << "Failed to present queue!";
		BREAK;
	}

	m_currentframe = (m_currentframe + 1) % PersistanceLib::MAXFRAMESINFLIGHT;
	m_drawingstarted = false;

}


void PersistanceVk::CreateAllocator()
{
	m_vmaalloccreateinfo.device = m_device;
	m_vmaalloccreateinfo.physicalDevice = m_physicaldevice;
	m_vmaalloccreateinfo.instance = m_instance;
	m_vmaalloccreateinfo.vulkanApiVersion = VK_API_VERSION_1_0;
	m_vmaalloccreateinfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;

	m_vmafunctions = {};
	m_vmafunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
	m_vmafunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

	m_vmaalloccreateinfo.pVulkanFunctions = &m_vmafunctions;
	
	if (vmaCreateAllocator(&m_vmaalloccreateinfo, &m_vmaallocator) != VK_SUCCESS) 
	{
		BREAK(0);
	}

}

void PersistanceVk::CleanAllocator()
{
	vmaDestroyAllocator(m_vmaallocator);
}

void PersistanceVk::CreateImage(const uint32_t& width, const uint32_t height, VkFormat format, VkImageTiling tiling, const VkImageUsageFlags& usage, const VkMemoryPropertyFlags& properties, VkImage& image, VmaAllocation& allocation, VkSharingMode sharingmode)
{

	VkImageCreateInfo imageinfo{};
	imageinfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageinfo.imageType = VK_IMAGE_TYPE_2D;
	imageinfo.extent.width = width;
	imageinfo.extent.height = height;
	imageinfo.extent.depth = 1;
	imageinfo.mipLevels = 1;
	imageinfo.arrayLayers = 1;
	imageinfo.format = format;
	imageinfo.tiling = tiling;
	imageinfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageinfo.usage = usage;
	imageinfo.sharingMode = sharingmode;
	imageinfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageinfo.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT; //temporary solution, This means that the framebuffer image can have the same format as a texture, but change to that of the swapchain

	std::array<uint32_t, 2> queuefamilyindices;									  // so I can also write to it, or thats my theory at least, lets give it a go for now.

	if (sharingmode & VK_SHARING_MODE_CONCURRENT)
	{
		queuefamilyindices =
		{
			m_queuefamilyindices.graphicsfamily,
			m_queuefamilyindices.transferfamily

		};

		imageinfo.queueFamilyIndexCount = static_cast<uint32_t>(queuefamilyindices.size());
		imageinfo.pQueueFamilyIndices = queuefamilyindices.data();



	}
	else
	{
		imageinfo.queueFamilyIndexCount = 1;
		imageinfo.pQueueFamilyIndices = &m_queuefamilyindices.graphicsfamily;
	}

	VmaAllocationCreateInfo allocationcreateinfo{};
	allocationcreateinfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocationcreateinfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
	allocationcreateinfo.priority = 1.0;



	if (vmaCreateImage(m_vmaallocator, &imageinfo, &allocationcreateinfo, &image, &allocation, nullptr) != VK_SUCCESS) 
	{
		std::cout << "Failed to create image!";
		BREAK(0);
	}


	

}

void PersistanceVk::CreateBuffer(const VkDeviceSize& size, VkBufferUsageFlags usageflags, VkMemoryPropertyFlags properties, VkBuffer& buffer, VmaAllocation& allocation, VkSharingMode sharingmode)
{
	VkBufferCreateInfo buffercreateinfo{};
	buffercreateinfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffercreateinfo.size = size;

	buffercreateinfo.usage = usageflags;
	buffercreateinfo.sharingMode = sharingmode;
	std::array<uint32_t, 2> queuefamilyindices;
	if (sharingmode & VK_SHARING_MODE_CONCURRENT)
	{
		queuefamilyindices =
		{
			m_queuefamilyindices.graphicsfamily,
			m_queuefamilyindices.transferfamily

		};

		buffercreateinfo.queueFamilyIndexCount = static_cast<uint32_t>(queuefamilyindices.size());
		buffercreateinfo.pQueueFamilyIndices = queuefamilyindices.data();

	}
	

	VmaAllocationCreateInfo allocationcreateinfo{};
	allocationcreateinfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocationcreateinfo.minAlignment = 0;
	allocationcreateinfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
		VMA_ALLOCATION_CREATE_MAPPED_BIT;

	
	if (vmaCreateBuffer(m_vmaallocator, &buffercreateinfo, &allocationcreateinfo, &buffer, &allocation, nullptr))
	{
		std::cout << "Failed to create buffer";
		BREAK(0);

	}

}



void PersistanceVk::CreateSyncObjects()
{
	VkSemaphoreCreateInfo semaphorecreateinfo{};
	semaphorecreateinfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fencecreateinfo{};

	fencecreateinfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fencecreateinfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	s_imageavailable.resize(PersistanceLib::MAXFRAMESINFLIGHT);
	s_renderfinished.resize(PersistanceLib::MAXFRAMESINFLIGHT);
	f_inflightfence.resize(PersistanceLib::MAXFRAMESINFLIGHT);
	f_imagesinflight.resize(m_swapchainimages.size(), VK_NULL_HANDLE);


	for (int i = 0; i < PersistanceLib::MAXFRAMESINFLIGHT; i++)
	{
		if (vkCreateSemaphore(m_device, &semaphorecreateinfo, nullptr, &s_imageavailable[i]) != VK_SUCCESS || vkCreateSemaphore(m_device, &semaphorecreateinfo, nullptr, &s_renderfinished[i]) != VK_SUCCESS || vkCreateFence(m_device, &fencecreateinfo, nullptr, &f_inflightfence[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Semaphores or fences could not be initialized!");
		}

	}
}

void PersistanceVk::CleanUpSwapchain()
{
	for (auto framebuffer : m_swapchainframebuffers)
	{
		vkDestroyFramebuffer(m_device, framebuffer, nullptr);
	}
	for (const auto& imageviews : m_swapchainimageviews)
	{
		vkDestroyImageView(m_device, imageviews, nullptr);

	}
	vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

}


bool PersistanceVk::CheckValidationLayers()
{

	uint32_t validationlayercount = -1;

	vkEnumerateInstanceLayerProperties(&validationlayercount, nullptr);

	std::vector<VkLayerProperties> availablevalidationlayers(validationlayercount);

	vkEnumerateInstanceLayerProperties(&validationlayercount, availablevalidationlayers.data());

	for (const auto& layername : m_validationlayers)
	{
		bool foundlayer = false;

		for (const auto& layerproperties : availablevalidationlayers)
		{
		

			if (strcmp(layername, layerproperties.layerName) == 0) {
				foundlayer = true;
				break;
			}

		}


		if (!foundlayer)
		{
			return false;
		}


	}




	return true;
}

std::vector<const char*> PersistanceVk::GetRequiredInstanceExtensions()
{

	//glfw extensions
	uint32_t glfwextensioncount = -1;
	const char** glfwextensions;
	glfwextensions = glfwGetRequiredInstanceExtensions(&glfwextensioncount);
	std::vector<const char*> requiredextensions;
	requiredextensions.reserve(glfwextensioncount);

	for (unsigned int i = 0; i < glfwextensioncount; i++)
	{
		requiredextensions.emplace_back(glfwextensions[i]);


	}

	requiredextensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);


	if (enablevalidationlayers)
	{
		requiredextensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	}



	return requiredextensions;
}

VKAPI_ATTR VkBool32 VKAPI_CALL PersistanceVk::DebugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{

	std::cerr<< "Validation layer: " << pCallbackData->pMessage << std::endl;


	return VK_FALSE;
}

void PersistanceVk::SetDebugCreateInfoStructVariables(VkDebugUtilsMessengerCreateInfoEXT& createinfo)
{
	createinfo = {};
	createinfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createinfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createinfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createinfo.pfnUserCallback = DebugCallBack;
	createinfo.pUserData = nullptr;



}

void PersistanceVk::SelectPhysicalDevice()
{

	uint32_t physicaldevicecount = -1;

	vkEnumeratePhysicalDevices(m_instance, &physicaldevicecount, nullptr);

	if (physicaldevicecount == 0)
	{
		throw std::runtime_error("No GPU with Vulkan Support Found!");

	}
	std::vector<VkPhysicalDevice> physicaldevices(physicaldevicecount);

	vkEnumeratePhysicalDevices(m_instance, &physicaldevicecount, physicaldevices.data());

	uint32_t score = 0;
	uint32_t bestscore = 0;

	for (unsigned int i = 0; i < physicaldevicecount; i++)
	{
		QueueFamilyIndices indices = FindQueueFamilies(physicaldevices[i]);

		bool haspresentfamilyindex;

		VkPhysicalDeviceProperties deviceproperties;
		
		if (RateDevice(physicaldevices[i], score, haspresentfamilyindex, &deviceproperties))
		{

			if (score > bestscore)

			{
				bestscore = score;

				m_physicaldevice = physicaldevices[i];
				m_queuefamilyindices = indices;
				
				if (enablevalidationlayers)
				{
					std::cout << "\n  Using Device: " << deviceproperties.deviceName << "\n \n";
					
				}


			}


		}


		if (m_physicaldevice == nullptr)
		{
			throw std::runtime_error("Failed to find GPU");

		}

	}







}

bool PersistanceVk::RateDevice(VkPhysicalDevice& physicaldevice, uint32_t& scorehandle, bool& presentfamily, VkPhysicalDeviceProperties* propertieshandle)
{

	

	uint32_t score = 0;

	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(physicaldevice, &properties);

	if (propertieshandle != nullptr)
	{
		*propertieshandle = properties;
	}

	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(physicaldevice, &features);


	if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		score += 1000;

	}

	if (!features.geometryShader)
	{
		return false;

	}


	
	

	auto indices = FindQueueFamilies(physicaldevice);
	
	const bool& requiredextensionsupport = DeviceExtensionSupport(physicaldevice);

	if (indices.graphicsfamily != -1)
	{
		score += 100;


	}

	if (indices.presentfamily != -1)
	{
		score += 100;
		presentfamily = true;
	}

	if (indices.computefamily != -1)
	{
		score += 100;


	}

	if (indices.transferfamily != -1)
	{
		score += 100;

	}

	bool isswapchainadequate = false;


	if (!requiredextensionsupport)
	{
		return false;

	}

	else
	{

		SwapChainSupportDetails swapchainsupport = QuerySwapChainSupport(physicaldevice);

		isswapchainadequate = !swapchainsupport.surfaceformat.empty() && !swapchainsupport.presentmode.empty();


	}


	if (!isswapchainadequate)
	{
		return false;

	}
	

	scorehandle = score;


	return true;
}

QueueFamilyIndices PersistanceVk::FindQueueFamilies(VkPhysicalDevice& physicaldevice)
{
	QueueFamilyIndices indices;

	uint32_t familycount;

	vkGetPhysicalDeviceQueueFamilyProperties(physicaldevice, &familycount, nullptr);

	std::vector<VkQueueFamilyProperties>familyproperties(familycount);

	vkGetPhysicalDeviceQueueFamilyProperties(physicaldevice, &familycount, familyproperties.data());

	
	VkBool32 presentsupport = false;
	for (unsigned int i = 0; i < familycount; i++)
	{
		if (familyproperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsfamily = i;


			vkGetPhysicalDeviceSurfaceSupportKHR(physicaldevice, i, m_surface, &presentsupport);
			if (presentsupport)
			{
				indices.presentfamily = i;

			}

		}

		
		else if (familyproperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			indices.computefamily = i;
		}
		
		else if (familyproperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT && !(familyproperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
		{
			indices.transferfamily = i;

		}


		

	}


	return indices;
}

bool PersistanceVk::DeviceExtensionSupport(VkPhysicalDevice& physicaldevice)
{
	uint32_t extensioncount;

	

	vkEnumerateDeviceExtensionProperties(physicaldevice, nullptr, &extensioncount, nullptr);

	std::vector<VkExtensionProperties> availableextensions(extensioncount);

	vkEnumerateDeviceExtensionProperties(physicaldevice, nullptr, &extensioncount, availableextensions.data());

	std::set<std::string> requiredextensions = {m_deviceextensions.begin(), m_deviceextensions.end()};

	for(const auto& extension : availableextensions)
	{
		requiredextensions.erase(extension.extensionName);


	}






	return requiredextensions.empty();
}

void PersistanceVk::CreateLogicalDevice()
{


	std::vector<VkDeviceQueueCreateInfo> queuecreateinfos;

	std::set<uint32_t>uniquequeuefamilies =
	{
		m_queuefamilyindices.graphicsfamily,
		m_queuefamilyindices.presentfamily,
		m_queuefamilyindices.transferfamily

	};
	queuecreateinfos.reserve(uniquequeuefamilies.size());


	float queuepriority = 1.0;
	for (const auto& familyqueue : uniquequeuefamilies)
	{

		
		VkDeviceQueueCreateInfo devicequeuecreateinfo{};
		devicequeuecreateinfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		devicequeuecreateinfo.queueFamilyIndex = familyqueue;
		devicequeuecreateinfo.queueCount = 1;


		devicequeuecreateinfo.pQueuePriorities = &queuepriority;


		queuecreateinfos.emplace_back(devicequeuecreateinfo);



	}
	

	VkPhysicalDeviceFeatures devicefeatures{};
	//With this struct we can set which device features we want to use. 
	//Perhaps it would be smart to be able to change that through 
	//some level of abstraction so the user can simply and cleanly set whichever 
	// device features they would like to use.

	devicefeatures.samplerAnisotropy = VK_TRUE;
	//in this case I'm only using this one ^ 

	//We should add a check that allows or doesnt allow device features depending on what the hardware is capable of.
	//Since this is designed to make games, however, it is safe to asume that most, if not all, devices running this
	//program should at LEAST be able to use samplerAnisotropy. Still wouldnt be bad to include checks. 


	VkDeviceCreateInfo devicecreateinfo{};

	devicecreateinfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	devicecreateinfo.queueCreateInfoCount = static_cast<uint32_t>(queuecreateinfos.size());
	devicecreateinfo.pQueueCreateInfos = queuecreateinfos.data();


	devicecreateinfo.pEnabledFeatures = &devicefeatures;

	devicecreateinfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceextensions.size());
	devicecreateinfo.ppEnabledExtensionNames = m_deviceextensions.data();
	
	if (enablevalidationlayers) 
	{
		devicecreateinfo.enabledLayerCount = static_cast<uint32_t>(m_validationlayers.size());
		devicecreateinfo.ppEnabledLayerNames = m_validationlayers.data();
	}
	else 
	{
		devicecreateinfo.enabledLayerCount = 0;
	}



	if (vkCreateDevice(m_physicaldevice, &devicecreateinfo, nullptr, &m_device) != VK_SUCCESS)
	{
		throw std::runtime_error("Could not create logical device");

	}

	vkGetDeviceQueue(m_device, m_queuefamilyindices.graphicsfamily, 0, &m_graphicsqueue);
	vkGetDeviceQueue(m_device, m_queuefamilyindices.presentfamily, 0, &m_presentqueue);
	vkGetDeviceQueue(m_device, m_queuefamilyindices.transferfamily, 0, &m_transferqueue);

}

void PersistanceVk::CreateSurface()
{
	if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface!");

	}

}

SwapChainSupportDetails PersistanceVk::QuerySwapChainSupport(VkPhysicalDevice& physicaldevice)
{
	SwapChainSupportDetails details{};

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicaldevice, m_surface, &details.surfacecapabilities);

	uint32_t formatcount = 0;
	
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicaldevice, m_surface, &formatcount, nullptr);

	if (formatcount != 0)
	{
		details.surfaceformat.resize(formatcount);

		vkGetPhysicalDeviceSurfaceFormatsKHR(physicaldevice, m_surface, &formatcount, details.surfaceformat.data());
	}

	uint32_t presentcount = 0;

	vkGetPhysicalDeviceSurfacePresentModesKHR(physicaldevice, m_surface, &presentcount, nullptr);

	if (presentcount != 0)
	{

		details.presentmode.resize(presentcount);

		vkGetPhysicalDeviceSurfacePresentModesKHR(physicaldevice, m_surface, &presentcount, details.presentmode.data());

	}




	return details;
}

VkSurfaceFormatKHR PersistanceVk::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableformats)
{

	for (const auto& format : availableformats)
	{

		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return format;

		}




	}

	return availableformats[0];



}

VkPresentModeKHR PersistanceVk::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablepresentmodes)
{

	for (const auto& presentmode : availablepresentmodes)
	{
		if (presentmode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return presentmode;
			
		}


	}

	return VK_PRESENT_MODE_FIFO_KHR;

}

VkExtent2D PersistanceVk::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfacecapabilities)
{


	if (surfacecapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{

		return surfacecapabilities.currentExtent;


	}
	else
	{
		int width;
		int height;

		glfwGetFramebufferSize(m_window, &width, &height);


		VkExtent2D actualextent =
		{
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualextent.width = std::clamp(actualextent.width, surfacecapabilities.minImageExtent.width, surfacecapabilities.maxImageExtent.width);
		actualextent.height = std::clamp(actualextent.height, surfacecapabilities.minImageExtent.height, surfacecapabilities.maxImageExtent.height);

		return actualextent;

	}




}

std::vector<char> PersistanceVk::ReadFile(const char* filepath)
{
	std::ifstream file(filepath, std::ios::ate | std::ios::binary);

;
	if (!file.is_open())
	{
		std::cout << filepath << "\n";


		throw std::runtime_error("failed to read the file");

	}


	size_t filesize = (size_t)file.tellg();

	std::vector<char> buffer(filesize);


	file.seekg(0);
	file.read(buffer.data(), filesize);

	file.close();

	return buffer;


	
	
}

VkShaderModule PersistanceVk::CreateShaderModule(const std::vector<char>& shaderfile)
{

	VkShaderModuleCreateInfo createinfo{};
	createinfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createinfo.codeSize = shaderfile.size();
	createinfo.pCode = reinterpret_cast<const uint32_t*> (shaderfile.data());

	VkShaderModule shadermodule;
	
	if (vkCreateShaderModule(m_device, &createinfo, nullptr, &shadermodule) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create shader module");
	}
	
	return shadermodule;


}

VkCommandBuffer PersistanceVk::BeginSingleTimeCommands(VkCommandPool& commandpool, const VkCommandBufferLevel& level)
{

	VkCommandBufferAllocateInfo allocinfo{};
	allocinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocinfo.commandBufferCount = 1;
	allocinfo.commandPool = commandpool;
	allocinfo.level = level;

	VkCommandBuffer commandbuffer;

	if (vkAllocateCommandBuffers(m_device, &allocinfo, &commandbuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate command buffer");
	}

	VkCommandBufferBeginInfo begininfo{};
	begininfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begininfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandbuffer, &begininfo);

	

	return commandbuffer;
}

void PersistanceVk::EndSingleTimeCommands(VkCommandBuffer& commandbuffer, const VkCommandPool& commandpool, const VkQueue& submitqueue)
{

	vkEndCommandBuffer(commandbuffer);

	VkSubmitInfo submitinfo{};
	submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitinfo.commandBufferCount = 1;
	submitinfo.pCommandBuffers = &commandbuffer;
	

	vkQueueSubmit(submitqueue, 1, &submitinfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(submitqueue);
	
	vkFreeCommandBuffers(m_device, commandpool, 1, &commandbuffer);

}


VkExtent2D& PersistanceVk::GetSwapchainExtent()
{
	return m_swapchainextent;
}

VkFormat& PersistanceVk::GetSwapchainImageFormat()
{
	return m_swapchainimageformat;
}

uint32_t PersistanceVk::GetCurrentFrame()
{
	return m_currentframe;
}

VkCommandPool& PersistanceVk::GetGraphicsCommandPool()
{
	return m_graphicscommandpool;
}

VkCommandPool& PersistanceVk::GetTransferCommandPool()
{
	return m_transfercommandpool;
}




void PersistanceVk::RecreateSwapchain(uint32_t renderpasshandle)
{
	int width = 0;
	int height = 0;

	glfwGetWindowSize(m_window, &width, &height);

	while (width == 0 || height == 0)
	{
		glfwGetWindowSize(m_window, &width, &height);
		glfwWaitEvents();

	}
	

	vkDeviceWaitIdle(m_device);

	CleanUpSwapchain();

	CreateSwapChain();
	CreateImageViews();
	CreateSwapchainFramebuffers(renderpasshandle);

}





void PersistanceVk::CopyBuffer(VkBuffer& srcbuffer, VkBuffer& dstbuffer, VkDeviceSize size, VkCommandPool& commandpool, VkQueue& submitqueue)
{
	
	VkCommandBuffer commandbuffer = BeginSingleTimeCommands(commandpool);


	VkBufferCopy copyregion{};
	copyregion.srcOffset = 0;
	copyregion.dstOffset = 0;
	copyregion.size = size;
	vkCmdCopyBuffer(commandbuffer, srcbuffer, dstbuffer, 1, &copyregion);

	EndSingleTimeCommands(commandbuffer, commandpool, submitqueue);


}

uint32_t PersistanceVk::FindMemoryType(uint32_t typefilter, VkMemoryPropertyFlags flags)
{
	VkPhysicalDeviceMemoryProperties memproperties;

	vkGetPhysicalDeviceMemoryProperties(m_physicaldevice, &memproperties);

	for (uint32_t i = 0; i < memproperties.memoryTypeCount; i++)
	{
		if (typefilter & (i << i) && (memproperties.memoryTypes[i].propertyFlags & flags) == flags)
		{
			return i;
		}

	}
	throw std::runtime_error("Failed to find a suitable memory type!");


	return 0;
}



void PersistanceVk::TransitionImageLayout(VkImage& image, const VkFormat& format, VkImageLayout oldlayout, VkImageLayout newlayout, VkCommandPool& commandpool, VkQueue submitqueue)
{
	VkCommandBuffer commandbuffer = BeginSingleTimeCommands(commandpool);


	VkPipelineStageFlags srcstage;
	VkPipelineStageFlags dststage;



	VkImageMemoryBarrier membarrier{};
	membarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	membarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	membarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	membarrier.oldLayout = oldlayout;
	membarrier.newLayout = newlayout;

	membarrier.image = image;
	membarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	membarrier.subresourceRange.baseArrayLayer = 0;
	membarrier.subresourceRange.layerCount = 1;
	membarrier.subresourceRange.baseMipLevel = 0;
	membarrier.subresourceRange.levelCount = 1;

	if (oldlayout == VK_IMAGE_LAYOUT_UNDEFINED && newlayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{

		membarrier.srcAccessMask = 0; 
		membarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; 

		srcstage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dststage = VK_PIPELINE_STAGE_TRANSFER_BIT;


	}
	else if (oldlayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newlayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		membarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; 
		membarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		srcstage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dststage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;


	}
	else
	{
		throw std::invalid_argument("Layout unsupported!");

	}


	vkCmdPipelineBarrier(
		commandbuffer,
		srcstage, //todo
		dststage, //todo
		0, 0,
		nullptr, 0,
		nullptr, 1,
		&membarrier
	);

	EndSingleTimeCommands(commandbuffer, commandpool, submitqueue);


}

void PersistanceVk::CopyBuffertoImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height, VkCommandPool& commandpool, VkQueue& submitqueue)
{
	VkCommandBuffer commandbuffer = BeginSingleTimeCommands(commandpool);
	
	
	VkBufferImageCopy imgcopy{};
	imgcopy.bufferOffset = 0;
	imgcopy.bufferImageHeight = 0;
	imgcopy.bufferRowLength = 0;

	imgcopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imgcopy.imageSubresource.baseArrayLayer = 0;
	imgcopy.imageSubresource.mipLevel = 0;
	imgcopy.imageSubresource.layerCount = 1;

	imgcopy.imageOffset = { 0, 0, 0 };
	imgcopy.imageExtent = { width, height, 1 };


	vkCmdCopyBufferToImage(commandbuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imgcopy);
	
	EndSingleTimeCommands(commandbuffer, commandpool, submitqueue);

}

VkImageView PersistanceVk::CreateImageView(VkImage& image, VkFormat format, VkImageAspectFlags imageaspect)
{
	VkImageView imageview;

	VkImageViewCreateInfo viewinfo{};
	viewinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewinfo.image = image;
	viewinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewinfo.format = format;
	viewinfo.subresourceRange.aspectMask = imageaspect;
	viewinfo.subresourceRange.baseArrayLayer = 0;
	viewinfo.subresourceRange.layerCount = 1;
	viewinfo.subresourceRange.baseMipLevel = 0;
	viewinfo.subresourceRange.levelCount = 1;

	if (vkCreateImageView(m_device, &viewinfo, nullptr, &imageview) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create texture image view");

	}
	

	return imageview;
}

RenderPassHandle PersistanceVk::CreateRenderPassHandle()
{
	uint32_t handle = m_renderpasscount++;

	mh_renderpasses.emplace_back(RenderPass());

	return handle;
}

uint32_t PersistanceVk::CreateRenderPassColorAttachment(RenderPassHandle handle, VkFormat format, VkSampleCountFlagBits imagesamples, VkAttachmentLoadOp loadop, VkAttachmentStoreOp storeop, VkImageLayout initialimagelayout, VkImageLayout finalimagelayout)
{
	int index = mh_renderpasses.at(handle).colorattachments.size();
	mh_renderpasses.at(handle).colorattachments.push_back(RenderPassAttachment());

	RenderPassAttachment& attachment = mh_renderpasses.at(handle).colorattachments[index];

	attachment.reference.attachment = index;
	attachment.reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	attachment.description.format = format;
	attachment.description.samples = imagesamples;
	attachment.description.loadOp = loadop;
	attachment.description.storeOp = storeop;
	attachment.description.initialLayout = initialimagelayout;
	attachment.description.finalLayout = finalimagelayout;

	attachment.description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	return index;

}

uint32_t PersistanceVk::CreateRenderPassDepthStencilAttachment(RenderPassHandle handle, VkFormat format, VkSampleCountFlagBits imagesamples, VkAttachmentLoadOp loadop, VkAttachmentStoreOp storeop, VkAttachmentLoadOp depthstencilloadop, VkAttachmentStoreOp depthstencilstoreop, VkImageLayout initialimagelayout, VkImageLayout finalimagelayout)
{
	int index = mh_renderpasses.at(handle).depthstencilattachments.size();
	mh_renderpasses.at(handle).depthstencilattachments.push_back(RenderPassAttachment());

	RenderPassAttachment& attachment = mh_renderpasses.at(handle).depthstencilattachments[index];

	attachment.reference.attachment = index;
	attachment.reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	attachment.description.format = format;
	attachment.description.samples = imagesamples;
	attachment.description.loadOp = loadop;
	attachment.description.storeOp = storeop;
	attachment.description.initialLayout = initialimagelayout;
	attachment.description.finalLayout = finalimagelayout;

	attachment.description.stencilLoadOp = depthstencilloadop;
	attachment.description.stencilStoreOp = depthstencilstoreop;

	return index;
}

uint32_t PersistanceVk::CreateRenderPassInputAttachment(RenderPassHandle handle, VkFormat format, VkSampleCountFlagBits imagesamples, VkAttachmentLoadOp loadop, VkAttachmentStoreOp storeop, VkAttachmentLoadOp depthstencilloadop, VkAttachmentStoreOp depthstencilstoreop, VkImageLayout initialimagelayout, VkImageLayout finalimagelayout)
{
	int index = mh_renderpasses.at(handle).inputattachments.size();
	mh_renderpasses.at(handle).inputattachments.push_back(RenderPassAttachment());

	RenderPassAttachment& attachment = mh_renderpasses.at(handle).inputattachments[index];

	attachment.reference.attachment = index;
	attachment.reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	attachment.description.format = format;
	attachment.description.samples = imagesamples;
	attachment.description.loadOp = loadop;
	attachment.description.storeOp = storeop;
	attachment.description.initialLayout = initialimagelayout;
	attachment.description.finalLayout = finalimagelayout;

	attachment.description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	return index;
}



uint32_t PersistanceVk::CreateRenderpassAttachment(RenderPassHandle handle, VkImageLayout attachmentlayout, VkFormat format, VkSampleCountFlagBits imagesamples, VkAttachmentLoadOp loadop, VkAttachmentStoreOp storeop, VkImageLayout initialimagelayout, VkImageLayout finalimagelayout)
{
	int index = mh_renderpasses.at(handle).attachments.size();
	mh_renderpasses.at(handle).attachments.push_back(RenderPassAttachment());

	RenderPassAttachment& attachment = mh_renderpasses.at(handle).attachments[index];
	
	attachment.reference.attachment = index;
	attachment.reference.layout = attachmentlayout;
	
	attachment.description.format = format;
	attachment.description.samples = imagesamples;
	attachment.description.loadOp = loadop;
	attachment.description.storeOp = storeop;
	attachment.description.initialLayout = initialimagelayout;
	attachment.description.finalLayout = finalimagelayout;

	attachment.description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	return index;

}

uint32_t PersistanceVk::CreateSubpassDescription(RenderPassHandle handle, const uint32_t* colorattachmentindices, size_t colorattachmentcount, const uint32_t depthandstencilattachmentindex, const uint32_t* inputattachmentindices, const uint32_t inputattachmentcount, const uint32_t* preserveattachmentindices, const uint32_t preservedattachmentcount)
{
	uint32_t descriptionindex = mh_renderpasses.at(handle).subpassdescription.size();
	mh_renderpasses.at(handle).subpassdescription.push_back(VkSubpassDescription());
	VkSubpassDescription& description = mh_renderpasses.at(handle).subpassdescription.at(descriptionindex);

	auto& attachments = mh_renderpasses.at(handle).attachments;



	if (depthandstencilattachmentindex != UINT32_MAX) {
		description.pDepthStencilAttachment = &attachments[depthandstencilattachmentindex].reference;
	}

	
	// Setting up attachment vectors
	std::vector<VkAttachmentReference> colorattachments;
	colorattachments.reserve(colorattachmentcount);
	std::vector<VkAttachmentReference> inputattachments;
	inputattachments.reserve(inputattachmentcount);
	


	
	// Assign attachment references to designated vectors
	/*for (int i = 0; i < colorattachmentcount; i++) 
	{
		VkAttachmentReference& attachmentref = attachments[colorattachmentindices[i]].reference;
		colorattachments.push_back(attachmentref);
		
	}
	for (int i = 0; i < inputattachmentcount; i++) 
	{
		VkAttachmentReference& attachmentref = attachments[inputattachmentindices[i]].reference;
		inputattachments.push_back(attachmentref);
	
	}*/

	//assigning attachments to the subpass description
	description.colorAttachmentCount = (uint32_t)mh_renderpasses.at(handle).colorattachments.size();
	description.pColorAttachments = &mh_renderpasses.at(handle).colorattachments[0].reference;
	description.inputAttachmentCount = inputattachmentcount;
	description.pInputAttachments = inputattachments.data();
	description.preserveAttachmentCount = preservedattachmentcount;
	description.pPreserveAttachments = preserveattachmentindices;


	

	return descriptionindex;


}

uint32_t PersistanceVk::CreateSubpassDependency(RenderPassHandle handle, uint32_t srcsubpass, uint32_t dstsubpass, VkPipelineStageFlags srcstagemask, VkPipelineStageFlags dststagemask, VkAccessFlags srcaccessmask, VkAccessFlags dstaccessmask)
{
	uint32_t dependencyindex = mh_renderpasses.at(handle).subpassdependencies.size();
	mh_renderpasses.at(handle).subpassdependencies.push_back(VkSubpassDependency());
	VkSubpassDependency& dependency = mh_renderpasses.at(handle).subpassdependencies.at(dependencyindex);


	dependency.srcSubpass = srcsubpass;
	dependency.srcStageMask = srcstagemask;
	dependency.srcAccessMask = srcaccessmask;

	dependency.dstSubpass = dstsubpass;
	dependency.dstStageMask = dststagemask;
	dependency.dstAccessMask = dstaccessmask;



	return dependencyindex;

}

void PersistanceVk::CreateRenderPass(RenderPassHandle handle, const uint32_t* attachmentindicies, uint32_t attachmentcount, const uint32_t* subpassdescriptionindicies, uint32_t subpassdescriptioncount, const uint32_t* subpassdependencyindices, uint32_t subpassdependencycount)
{
	std::vector<VkAttachmentDescription> attachments;
	attachments.reserve(attachmentcount);

	for (int i = 0; i < attachmentcount; i++)
	{
		attachments.emplace_back(mh_renderpasses.at(handle).colorattachments[attachmentindicies[i]].description);

	}



	VkRenderPassCreateInfo renderpasscreateinfo{};
	renderpasscreateinfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderpasscreateinfo.attachmentCount = attachmentcount;
	renderpasscreateinfo.pAttachments = attachments.data();
	renderpasscreateinfo.subpassCount = (uint32_t)mh_renderpasses.at(handle).subpassdescription.size();
	renderpasscreateinfo.pSubpasses = mh_renderpasses.at(handle).subpassdescription.data();

	renderpasscreateinfo.dependencyCount = (uint32_t)mh_renderpasses.at(handle).subpassdependencies.size();
	renderpasscreateinfo.pDependencies = mh_renderpasses.at(handle).subpassdependencies.data();


	if (vkCreateRenderPass(m_device, &renderpasscreateinfo, nullptr, &mh_renderpasses.at(handle).renderpass) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create render pass!");

	}




}

void PersistanceVk::CleanRenderPass()
{
	for (RenderPass pass : mh_renderpasses) 
	{
		vkDestroyRenderPass(m_device, pass.renderpass, nullptr);
	}

}


DescriptorSetLayoutHandle PersistanceVk::CreateDescriptorSetLayoutHandle()
{
	uint32_t handle = m_dslhandlecount++;

	mh_descriptorsetlayouts.emplace_back(DescriptorSetLayout());

	return handle;
}

void PersistanceVk::AddDescriptorSetLayoutBinding(DescriptorSetHandle handle, VkDescriptorSetLayoutBinding& binding)
{
	mh_descriptorsetlayouts.at(handle).bindings.push_back(binding);
}

void PersistanceVk::AddDescriptorSetLayoutBinding(DescriptorSetHandle handle, uint32_t bindingidx, VkDescriptorType descriptortype, VkShaderStageFlagBits shaderstage)
{
	VkDescriptorSetLayoutBinding binding{};
	binding.binding = bindingidx;
	binding.descriptorCount = 1;
	binding.descriptorType = descriptortype;
	binding.stageFlags = shaderstage;
	binding.pImmutableSamplers = 0;
	
	mh_descriptorsetlayouts.at(handle).bindings.push_back(binding);
	

}

void PersistanceVk::CreateDescriptorSetLayout(DescriptorSetHandle handle)
{

	//VkDescriptorSetLayout layout;

	VkDescriptorSetLayoutCreateInfo layoutinfo{};
	layoutinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutinfo.bindingCount = static_cast<uint32_t>(mh_descriptorsetlayouts.at(handle).bindings.size());
	layoutinfo.pBindings = mh_descriptorsetlayouts.at(handle).bindings.data();

	if (vkCreateDescriptorSetLayout(m_device, &layoutinfo, nullptr, &mh_descriptorsetlayouts.at(handle).layout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create descriptor set layout!");
	}

	//mh_descriptorsetlayouts.at(handle) = layout;
	

}

void PersistanceVk::CleanDescriptorSetLayout()
{
	for (int i = 0; i < mh_descriptorsetlayouts.size(); i++)
	{
		vkDestroyDescriptorSetLayout(m_device, mh_descriptorsetlayouts[i].layout, nullptr);
	}
}

GraphicsPipelineHandle PersistanceVk::CreateGraphicsPipelineHandle()
{
	uint32_t handle = m_pipelinecount++;

	mh_graphicspipelines.emplace_back(GraphicsPipeline());



	return handle;



}



void PersistanceVk::AddVertexStage(GraphicsPipelineHandle handle, const char* shaderpath)
{
	const auto shaderfile = ReadFile(shaderpath);
	mh_graphicspipelines.at(handle).shader.GetVertexModule() = CreateShaderModule(shaderfile);
	mh_graphicspipelines.at(handle).shader.AddVertexShaderStage();
}

void PersistanceVk::AddFragmentStage(GraphicsPipelineHandle handle, const char* shaderpath)
{
	const auto shaderfile = ReadFile(shaderpath);
	mh_graphicspipelines.at(handle).shader.GetFragmentModule() = CreateShaderModule(shaderfile);
	mh_graphicspipelines.at(handle).shader.AddFragmentShaderStage();
}

void PersistanceVk::CreateGraphicsPipelineLayout(GraphicsPipelineHandle graphicspipelinehandle, uint32_t descriptorsetbinding)
{

	VkPipelineLayoutCreateInfo pipelinelayoutcreateinfo{};
	pipelinelayoutcreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelinelayoutcreateinfo.setLayoutCount = 1;
	pipelinelayoutcreateinfo.pSetLayouts = &mh_descriptorsetlayouts.at(descriptorsetbinding).layout;
	pipelinelayoutcreateinfo.pushConstantRangeCount = 0;
	pipelinelayoutcreateinfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(m_device, &pipelinelayoutcreateinfo, nullptr, &mh_graphicspipelines.at(graphicspipelinehandle).layout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create pipeline layout!");
	}
}

void PersistanceVk::CreateGraphicsPipeline(GraphicsPipelineHandle handle, PipelineSettings& settings, const uint32_t renderpasshandle)
{
	std::vector<VkDynamicState> dynamicstates =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR

	};

	VkPipelineDynamicStateCreateInfo dynamicstatecreateinfo{};
	dynamicstatecreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicstatecreateinfo.dynamicStateCount = static_cast<uint32_t>(dynamicstates.size());
	dynamicstatecreateinfo.pDynamicStates = dynamicstates.data();


	//////////////////////////////////
	VkGraphicsPipelineCreateInfo pipelinecreateinfo{};
	

	pipelinecreateinfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelinecreateinfo.stageCount = static_cast<uint32_t>(mh_graphicspipelines.at(handle).shader.GetStages().size());
	pipelinecreateinfo.pStages = mh_graphicspipelines.at(handle).shader.GetStages().data();


	pipelinecreateinfo.pVertexInputState = &settings.GetVertexInputStateCreateInfo();
	pipelinecreateinfo.pInputAssemblyState = &settings.GetInputAssemblyStateCreateInfo();
	pipelinecreateinfo.pViewportState = &settings.Getviewportcreateinfo();
	pipelinecreateinfo.pRasterizationState = &settings.GetRasterCreateInfo();
	pipelinecreateinfo.pMultisampleState = &settings.GetMultisampleCreateInfo();
	pipelinecreateinfo.pColorBlendState = &settings.GetColorBlendCreateInfo();
	pipelinecreateinfo.pDepthStencilState = nullptr;


	pipelinecreateinfo.layout = mh_graphicspipelines.at(handle).layout;
	pipelinecreateinfo.pDynamicState = (settings.m_usedynamicstate) ? &dynamicstatecreateinfo : nullptr;
	pipelinecreateinfo.renderPass = mh_renderpasses.at(renderpasshandle).renderpass;
	pipelinecreateinfo.subpass = 0;

	pipelinecreateinfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelinecreateinfo.basePipelineIndex = -1;

	pipelinecreateinfo.pNext = nullptr;

	if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelinecreateinfo, nullptr, &mh_graphicspipelines.at(handle).pipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create the graphics pipeline");

	}

	DestroyShaders(handle);
	


}

void PersistanceVk::DestroyShaders(GraphicsPipelineHandle handle)
{
	
	vkDestroyShaderModule(m_device, mh_graphicspipelines.at(handle).shader.GetVertexModule(), nullptr);
	vkDestroyShaderModule(m_device, mh_graphicspipelines.at(handle).shader.GetFragmentModule(), nullptr);

	// compute one goes here.
}

void PersistanceVk::CleanGraphicsPipelines()
{
	for (GraphicsPipeline pipeline : mh_graphicspipelines) 
	{
		vkDestroyPipeline(m_device, pipeline.pipeline, nullptr);
		vkDestroyPipelineLayout(m_device, pipeline.layout, nullptr);
	}


}

FrameBufferHandle PersistanceVk::CreateFrameBuffersHandle()
{
	
	mh_framebuffers.emplace_back(Framebuffer());
	
	return m_framebuffercount++;

}

void PersistanceVk::CreateFramebufferImage(FrameBufferHandle handle)
{
	uint32_t imageidx = 0;
	int width = m_swapchainextent.width;
	int height = m_swapchainextent.height;
	VkFormat format = VK_FORMAT_R8G8B8A8_SRGB; //could cause problems since its not the same as the swapchain one.
	VkImageTiling tiling; tiling = VK_IMAGE_TILING_OPTIMAL;
	VkImageUsageFlags usageflags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	VkMemoryPropertyFlags memoryproperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;


	imageidx = (uint32_t)mh_framebuffers.at(handle).images.size();

	mh_framebuffers.at(handle).images.emplace_back(VkImage());
	////mh_framebuffers.at(handle).imagememory.emplace_back(VkDeviceMemory());
	mh_framebuffers.at(handle).allocations.emplace_back(VmaAllocation());

	VkImage& image = mh_framebuffers.at(handle).images[imageidx];
	VmaAllocation& allocation = mh_framebuffers.at(handle).allocations[imageidx];
	

	CreateImage(width, height, format, tiling, usageflags, memoryproperties, image, allocation);

	


	

}
 
void PersistanceVk::CreateFramebufferImage(FrameBufferHandle handle, int width, int height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageflags, VkMemoryPropertyFlags memoryproperties)
{
	uint32_t imageidx = 0;
	mh_framebuffers.at(handle).images.push_back(VkImage());
	mh_framebuffers.at(handle).allocations.push_back(VmaAllocation());


	imageidx = (uint32_t)mh_framebuffers.at(handle).images.size() - 1;

	CreateImage(width, height, format, tiling, usageflags, memoryproperties, mh_framebuffers.at(handle).images[imageidx], mh_framebuffers.at(handle).allocations[imageidx] , VK_SHARING_MODE_CONCURRENT);

}

void PersistanceVk::CreateFramebufferImageViews(FrameBufferHandle handle)
{
	size_t imagecount = mh_framebuffers.at(handle).images.size();

	mh_framebuffers.at(handle).imageviews.resize(imagecount);

	for (int i = 0; i < imagecount; i++)
	{
		mh_framebuffers.at(handle).imageviews[i] = CreateImageView(mh_framebuffers.at(handle).images[i], m_swapchainimageformat);

	}


}
void PersistanceVk::CreateFramebuffers(FrameBufferHandle handle, const uint32_t renderpasshandle)
{
	size_t imageviewcount = mh_framebuffers.at(handle).imageviews.size();
	mh_framebuffers.at(handle).framebuffers.resize(imageviewcount);
	//using a handle, this would be replaced by a stored vector of framebuffers being resized according to the amount of framebuffers we want to make.

	for (int i = 0; i < imageviewcount; i++)
	{

		VkImageView attachments[] = { mh_framebuffers.at(handle).imageviews[i] }; //separate this into another map

		VkFramebufferCreateInfo framebufferinfo{};
		framebufferinfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferinfo.renderPass = mh_renderpasses.at(renderpasshandle).renderpass;
		framebufferinfo.layers = 1;
		framebufferinfo.attachmentCount = 1;
		framebufferinfo.pAttachments = attachments;
		framebufferinfo.width = m_swapchainextent.width;
		framebufferinfo.height = m_swapchainextent.height;

		if (vkCreateFramebuffer(m_device, &framebufferinfo, nullptr, &mh_framebuffers.at(handle).framebuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create framebuffer");

		}
	}
}

void PersistanceVk::CleanFramebuffers()
{
	for (Framebuffer buffer : mh_framebuffers) 
	{
		for (int i = 0; i < buffer.framebuffers.size(); i++)
		{
			vkDestroyFramebuffer(m_device, buffer.framebuffers[i], nullptr);
		}

		for (int i = 0; i < buffer.imageviews.size(); i++)
		{
			vkDestroyImageView(m_device, buffer.imageviews[i], nullptr);
		}
		
		for (int i = 0; i < buffer.images.size(); i++) 
		{
			vmaDestroyImage(m_vmaallocator, buffer.images[i], buffer.allocations[i]);
		}
		/*The reason as to why imageviews and images are deleted sepperately is because the developer
		might not create an imageview for every image.*/

		
	}

}

uint32_t PersistanceVk::CreateTextureHandle()
{
	uint32_t handle = m_texturecount++;

	mh_textures.emplace_back(Texture());

	return handle;

}

void PersistanceVk::CreateTextureImage(TextureHandle handle, int width, int height)
{
	CreateImage(width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		mh_textures.at(handle).image, mh_textures.at(handle).allocation, VK_SHARING_MODE_CONCURRENT);

	mh_textures.at(handle).width = width;
	mh_textures.at(handle).height = height;

}

void PersistanceVk::CreateTextureImage(TextureHandle handle, const char* imagesrc)
{

	int width, height, bpp;

	stbi_uc* pixels = stbi_load(imagesrc, &width, &height, &bpp, STBI_rgb_alpha);
	VkDeviceSize buffersize = width * height * 4;

	if (!pixels)
	{
		throw std::runtime_error("Didnt find the texture!");
	}

	VkBuffer stagingbuffer;
	VkDeviceMemory stagingmem;
	VmaAllocation stagingalloc;

	CreateBuffer(buffersize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingbuffer, stagingalloc, VK_SHARING_MODE_CONCURRENT);


	void* data;
	

	vmaMapMemory(m_vmaallocator, stagingalloc, &data);
	memcpy(data, pixels, static_cast<uint32_t>(buffersize));
	vmaUnmapMemory(m_vmaallocator, stagingalloc);
	
	stbi_image_free(pixels);

	CreateImage(width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		mh_textures.at(handle).image, mh_textures.at(handle).allocation, VK_SHARING_MODE_CONCURRENT);


	TransitionImageLayout(mh_textures.at(handle).image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_transfercommandpool, m_transferqueue);

	CopyBuffertoImage(stagingbuffer, mh_textures.at(handle).image, static_cast<uint32_t>(width), static_cast<uint32_t>(height), m_transfercommandpool, m_transferqueue);

	TransitionImageLayout(mh_textures.at(handle).image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_graphicscommandpool, m_graphicsqueue);

	vmaDestroyBuffer(m_vmaallocator, stagingbuffer, stagingalloc);

	
}

void PersistanceVk::CreateTextureImageView(TextureHandle handle)
{
	mh_textures.at(handle).imageview = CreateImageView(mh_textures.at(handle).image, VK_FORMAT_R8G8B8A8_SRGB);
}

void PersistanceVk::AddImageToTexture(TextureHandle handle, const char* imagesrc)
{
	int width, height, bpp;

	stbi_uc* pixels = stbi_load(imagesrc, &width, &height, &bpp, STBI_rgb_alpha);
	VkDeviceSize buffersize = width * height * 4;

	if (!pixels)
	{
		throw std::runtime_error("Didnt find the texture!");
	}

	VkBuffer stagingbuffer;
	VkDeviceMemory stagingmem;
	VmaAllocation stagingalloc;

	CreateBuffer(buffersize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingbuffer, stagingalloc, VK_SHARING_MODE_CONCURRENT);

	void* data;
	
	vmaMapMemory(m_vmaallocator, stagingalloc, &data);
	memcpy(data, pixels, static_cast<uint32_t>(buffersize));
	vmaUnmapMemory(m_vmaallocator, stagingalloc);

	stbi_image_free(pixels);

	TransitionImageLayout(mh_textures.at(handle).image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_transfercommandpool, m_transferqueue);

	CopyBuffertoImage(stagingbuffer, mh_textures.at(handle).image, static_cast<uint32_t>(width), static_cast<uint32_t>(height), m_transfercommandpool, m_transferqueue);

	TransitionImageLayout(mh_textures.at(handle).image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_graphicscommandpool, m_graphicsqueue);

	
	vmaDestroyBuffer(m_vmaallocator, stagingbuffer, stagingalloc);
	
}

TextureSamplerHandle PersistanceVk::CreateTextureSamplerHandle()
{
	uint32_t handle = m_texturesamplercount++;

	mh_texturesamplers.emplace_back(VkSampler());
	
	return handle;
}

void PersistanceVk::CreateTextureSampler(TextureSamplerHandle handle, VkFilter magfilter, VkFilter minfilter, VkSamplerAddressMode addressmodeU, VkSamplerAddressMode addressmodeV, VkSamplerAddressMode addressmodeW, VkBorderColor bordercolor, VkSamplerMipmapMode mipmapmode, float miplodbias, float minlod, float maxlod, bool anisotropy)
{
	VkPhysicalDeviceProperties properties{};

	vkGetPhysicalDeviceProperties(m_physicaldevice, &properties);



	VkSamplerCreateInfo samplerinfo{};
	samplerinfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

	samplerinfo.magFilter = magfilter;
	samplerinfo.minFilter = minfilter;

	samplerinfo.addressModeU = addressmodeU;
	samplerinfo.addressModeV = addressmodeV;
	samplerinfo.addressModeW = addressmodeW;

	samplerinfo.borderColor = bordercolor;

	samplerinfo.mipmapMode = mipmapmode;
	samplerinfo.mipLodBias = miplodbias;
	samplerinfo.minLod = minlod;
	samplerinfo.maxLod = maxlod;

	samplerinfo.anisotropyEnable = anisotropy;
	samplerinfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;



	samplerinfo.unnormalizedCoordinates = VK_FALSE;
	samplerinfo.compareEnable = VK_FALSE;
	samplerinfo.compareOp = VK_COMPARE_OP_ALWAYS;

	if (vkCreateSampler(m_device, &samplerinfo, nullptr, &mh_texturesamplers.at(handle)) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create sampler");

	}



}

void PersistanceVk::CleanTextures()
{
	for (int i = 0; i < mh_textures.size(); i++) {
		vkDestroyImageView(m_device, mh_textures[i].imageview, nullptr);
		vmaDestroyImage(m_vmaallocator, mh_textures[i].image, mh_textures[i].allocation);
	}
	for (auto& sampler : mh_texturesamplers) 
	{
		vkDestroySampler(m_device, sampler, nullptr);
	}
	
}

BufferHandle PersistanceVk::CreateVertexBufferHandle()
{
	uint32_t handle = m_vertexbuffercount++;

	mh_vertexbuffers.emplace_back(Buffer());

	return handle;
}

void PersistanceVk::CreateVertexBuffer(BufferHandle handle, const void* buffer, size_t elementsize, uint32_t elementcount)
{
	VkDeviceSize buffersize = elementsize * elementcount;

	VkBuffer stagingbuffer;
	VmaAllocation stagingalloc;

	CreateBuffer(buffersize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingbuffer, stagingalloc, VK_SHARING_MODE_CONCURRENT);

	void* data;

	vmaMapMemory(m_vmaallocator, stagingalloc, &data);
	memcpy(data, buffer, buffersize);
	vmaUnmapMemory(m_vmaallocator, stagingalloc);

	CreateBuffer(buffersize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mh_vertexbuffers.at(handle).buffer, mh_vertexbuffers.at(handle).allocation, VK_SHARING_MODE_CONCURRENT);
	mh_vertexbuffers.at(handle).size = buffersize;

	CopyBuffer(stagingbuffer, mh_vertexbuffers.at(handle).buffer, buffersize, m_transfercommandpool, m_transferqueue);
	
	vmaDestroyBuffer(m_vmaallocator, stagingbuffer, stagingalloc);
}

void PersistanceVk::CleanVertexBuffers()
{
	for (auto& vertexbuffer : mh_vertexbuffers) 
	{
		vmaDestroyBuffer(m_vmaallocator, vertexbuffer.buffer, vertexbuffer.allocation);	
	}
}

uint32_t PersistanceVk::CreateIndexBufferHandle()
{
	uint32_t handle = m_indexbuffercount++;

	mh_indexbuffers.emplace_back(Buffer());
	
	return handle;
}

void PersistanceVk::CreateIndexBuffer(BufferHandle handle, void* buffer, uint32_t indexcount)
{
	VkDeviceSize buffersize = sizeof(uint32_t) * indexcount;

	VkBuffer stagingbuffer;
	VkDeviceMemory stagingbuffermem;
	VmaAllocation stagingalloc;

	CreateBuffer(buffersize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingbuffer, stagingalloc, VK_SHARING_MODE_CONCURRENT);

	void* data;

	vmaMapMemory(m_vmaallocator, stagingalloc, &data);
	memcpy(data, buffer, buffersize);
	vmaUnmapMemory(m_vmaallocator, stagingalloc);

	CreateBuffer(buffersize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mh_indexbuffers.at(handle).buffer, mh_indexbuffers.at(handle).allocation, VK_SHARING_MODE_CONCURRENT);
	mh_indexbuffers.at(handle).size = buffersize;

	CopyBuffer(stagingbuffer, mh_indexbuffers.at(handle).buffer, buffersize, m_transfercommandpool, m_transferqueue);

	vmaDestroyBuffer(m_vmaallocator, stagingbuffer, stagingalloc);
}

void PersistanceVk::CleanIndexBuffers()
{
	for (auto& indexbuffer : mh_indexbuffers)
	{
		vmaDestroyBuffer(m_vmaallocator, indexbuffer.buffer, indexbuffer.allocation);

	}
}

uint32_t PersistanceVk::CreateUniformBufferHandle()
{
	uint32_t handle = m_uniformbuffercount++;
	mh_uniformbuffers.emplace_back(UniformBuffer());
	return handle;
}

void PersistanceVk::CreateUniformBuffer(uint32_t handle, size_t buffersize)
{
	mh_uniformbuffers.at(handle).size = buffersize;
	UniformBuffer& buffer = mh_uniformbuffers.at(handle);

	for (int i = 0; i < PersistanceLib::MAXFRAMESINFLIGHT; i++)
	{
		CreateBuffer(buffer.size , VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, buffer.buffers[i], buffer.allocations[i], VK_SHARING_MODE_CONCURRENT);
		
		vmaMapMemory(m_vmaallocator, buffer.allocations[i], &buffer.memorymaps[i]);
	}
}

void PersistanceVk::UpdateUniformBuffer(uint32_t handle, const void* buffer, const uint32_t currentframe)
{

	UniformBuffer& uniformbuffer = mh_uniformbuffers.at(handle);
	

	memcpy(uniformbuffer.memorymaps[currentframe], buffer, uniformbuffer.size);

	

}

void PersistanceVk::CleanUniformBuffers()
{

	for (auto& uniformbuffer : mh_uniformbuffers) 
	{
		for (int j = 0; j < PersistanceLib::MAXFRAMESINFLIGHT; j++)
		{
			vmaUnmapMemory(m_vmaallocator, uniformbuffer.allocations[j]);
			vmaDestroyBuffer(m_vmaallocator, uniformbuffer.buffers[j], uniformbuffer.allocations[j]);

		}
	}

}




















