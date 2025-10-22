#include "Application.h"

void Application::SetUpDebugCallBack()
{

	if (!enablevalidationlayers)
	{
		return;
	}
	
	VkDebugUtilsMessengerCreateInfoEXT createinfo;
	SetDebugCreateInfoStructVariables(createinfo);

	/*if (CreateDebugUtilsMessengerEXT(m_instance, &createinfo, nullptr, &debugmessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}*/
	if (DebugUtilsMessengerEXT::Create(m_instance, &createinfo, nullptr, &debugmessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug messenger");

	}




}

void Application::CreateInstance()
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
	
	createinfo.enabledLayerCount = 0;

	if (vkCreateInstance(&createinfo, nullptr, &m_instance) != VK_SUCCESS)
	{
		std::cout << "Failed at Creating Instance \n";
		BREAK;

	}
	else
	{
		std::cout << "Created Application Instance\n";
		std::cout << "Application Name: " << appinfo.pApplicationName << "\nEngine: "<< appinfo.pEngineName<< "\n";
		

	}

}

void Application::CreateSwapChain()
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
		indices.graphicsfamily.value(), indices.presentfamily.value()
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

void Application::CreateImageViews()
{
	m_swapchainimageviews.resize(m_swapchainimages.size());

	for (int i = 0; i < m_swapchainimages.size(); i++)
	{
		
		m_swapchainimageviews[i] = CreateImageView(m_swapchainimages[i], m_swapchainimageformat);




	}




}

void Application::CreateRenderPass()
{
	VkAttachmentDescription colorattachment{};
	colorattachment.format = m_swapchainimageformat;
	colorattachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorattachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorattachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	colorattachment.stencilLoadOp= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorattachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	colorattachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorattachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


	VkAttachmentReference colorattachmentref{};
	colorattachmentref.attachment = 0;
	colorattachmentref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorattachmentref;


	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;

	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;

	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;



	VkRenderPassCreateInfo renderpasscreateinfo{};
	renderpasscreateinfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderpasscreateinfo.attachmentCount = 1;
	renderpasscreateinfo.pAttachments = &colorattachment;
	renderpasscreateinfo.subpassCount = 1;
	renderpasscreateinfo.pSubpasses = &subpass;

	renderpasscreateinfo.dependencyCount = 1;
	renderpasscreateinfo.pDependencies = &dependency;


	if (vkCreateRenderPass(m_device, &renderpasscreateinfo, nullptr, &m_renderpass) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create render pass!");

	}



}

void Application::CreateDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding layoutbinding{};
	layoutbinding.descriptorCount = 1;
	layoutbinding.binding = 0;
	layoutbinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutbinding.pImmutableSamplers = nullptr;
	layoutbinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;


	VkDescriptorSetLayoutCreateInfo layoutinfo{};
	layoutinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutinfo.bindingCount = 1;
	layoutinfo.pBindings = &layoutbinding;
	
	if (vkCreateDescriptorSetLayout(m_device, &layoutinfo, nullptr, &m_descriptorsetlayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create descriptor set layout!");
	}



}

void Application::CreateDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(MAXFRAMESINFLIGHT, m_descriptorsetlayout);
	VkDescriptorSetAllocateInfo allocateinfo{};
	allocateinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateinfo.descriptorPool = m_descriptorpool;
	allocateinfo.descriptorSetCount = static_cast<uint32_t>(MAXFRAMESINFLIGHT);
	allocateinfo.pSetLayouts = layouts.data();


	m_descriptorsets.resize(MAXFRAMESINFLIGHT);
	
	if (vkAllocateDescriptorSets(m_device, &allocateinfo, m_descriptorsets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate descriptor sets!");
	}

	for (int i = 0; i < MAXFRAMESINFLIGHT; i++)
	{
		VkDescriptorBufferInfo bufferinfo{};
		bufferinfo.buffer = m_uniformbuffers[i];
		bufferinfo.offset = 0;
		bufferinfo.range = sizeof(ModelViewProjectionBuffer);

		VkWriteDescriptorSet writedescriptor{};
		writedescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writedescriptor.descriptorCount = 1;
		writedescriptor.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writedescriptor.dstSet = m_descriptorsets[i];
		writedescriptor.dstBinding = 0;
		writedescriptor.dstArrayElement = 0;
		writedescriptor.pBufferInfo = &bufferinfo;
		writedescriptor.pImageInfo = nullptr;
		writedescriptor.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(m_device, 1, &writedescriptor, 0, nullptr);


	}



}

void Application::CreateGraphicsPipeline()
{
	const auto vertexshaderfile = ReadFile("res/Shaders/basicvert.spv");
	const auto fragmentshaderfile = ReadFile("res/Shaders/basicfrag.spv");

	VkShaderModule vertexmodule = CreateShaderModule(vertexshaderfile);
	VkShaderModule fragmentmodule = CreateShaderModule(fragmentshaderfile);

	VkPipelineShaderStageCreateInfo vertcreateinfo{};
	vertcreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertcreateinfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertcreateinfo.module = vertexmodule;
	vertcreateinfo.pName = "main";


	VkPipelineShaderStageCreateInfo fragcreateinfo{};
	fragcreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragcreateinfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragcreateinfo.module = fragmentmodule;
	fragcreateinfo.pName = "main";


	VkPipelineShaderStageCreateInfo shaderstages[] = {vertcreateinfo, fragcreateinfo};
	

	std::vector<VkDynamicState> dynamicstates =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR

	};

	VkPipelineDynamicStateCreateInfo dynamicstatecreateinfo{};
	dynamicstatecreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicstatecreateinfo.dynamicStateCount = static_cast<uint32_t>(dynamicstates.size());
	dynamicstatecreateinfo.pDynamicStates = dynamicstates.data();




	VkPipelineVertexInputStateCreateInfo inputvertexcreateinfo{};
	inputvertexcreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	
	inputvertexcreateinfo.vertexBindingDescriptionCount = 0;
	inputvertexcreateinfo.pVertexBindingDescriptions = nullptr;
	
	inputvertexcreateinfo.vertexAttributeDescriptionCount = 0;
	inputvertexcreateinfo.pVertexAttributeDescriptions = nullptr;

	auto bindingdescription = Vertex::GetBindingDescription();
	auto attributedescription = Vertex::GetAttributeDescription();


	inputvertexcreateinfo.vertexBindingDescriptionCount = 1;
	inputvertexcreateinfo.vertexAttributeDescriptionCount = attributedescription.size();

	inputvertexcreateinfo.pVertexBindingDescriptions = &bindingdescription;
	inputvertexcreateinfo.pVertexAttributeDescriptions = attributedescription.data();


	VkPipelineInputAssemblyStateCreateInfo inputassemblycreateinfo{};
	inputassemblycreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputassemblycreateinfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputassemblycreateinfo.primitiveRestartEnable = VK_FALSE;


	


	VkPipelineViewportStateCreateInfo viewportcreateinfo{};
	viewportcreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportcreateinfo.viewportCount = 1;
	viewportcreateinfo.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rastercreateinfo{};
	rastercreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rastercreateinfo.depthClampEnable = VK_FALSE;
	rastercreateinfo.polygonMode = VK_POLYGON_MODE_FILL;
	rastercreateinfo.lineWidth = 1.0f;
	rastercreateinfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rastercreateinfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rastercreateinfo.rasterizerDiscardEnable = VK_FALSE;

	rastercreateinfo.depthBiasClamp = VK_FALSE;
	rastercreateinfo.depthBiasEnable = VK_FALSE;
	rastercreateinfo.depthBiasSlopeFactor = 0.f;
	rastercreateinfo.depthBiasConstantFactor = 0.f;

	

	VkPipelineMultisampleStateCreateInfo multisamplecreateinfo{};
	multisamplecreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisamplecreateinfo.sampleShadingEnable = VK_FALSE;
	multisamplecreateinfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisamplecreateinfo.minSampleShading = 1.0f;
	multisamplecreateinfo.pSampleMask = nullptr;
	multisamplecreateinfo.alphaToCoverageEnable = VK_FALSE;
	multisamplecreateinfo.alphaToOneEnable = VK_FALSE;


	VkPipelineColorBlendAttachmentState colorblendattachment{};
	colorblendattachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	colorblendattachment.blendEnable = VK_TRUE;
	
	colorblendattachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorblendattachment.alphaBlendOp = VK_BLEND_OP_ADD;

	colorblendattachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorblendattachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

	colorblendattachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorblendattachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;


	VkPipelineColorBlendStateCreateInfo colorblendcreateinfo{};
	colorblendcreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorblendcreateinfo.attachmentCount = 1;
	colorblendcreateinfo.logicOpEnable = VK_FALSE;
	colorblendcreateinfo.logicOp = VK_LOGIC_OP_COPY;
	colorblendcreateinfo.pAttachments = &colorblendattachment;
	colorblendcreateinfo.blendConstants[0] = 0.0f;
	colorblendcreateinfo.blendConstants[1] = 0.0f;
	colorblendcreateinfo.blendConstants[2] = 0.0f;
	colorblendcreateinfo.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo pipelinelayoutcreateinfo{};
	pipelinelayoutcreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelinelayoutcreateinfo.setLayoutCount = 1;
	pipelinelayoutcreateinfo.pSetLayouts = &m_descriptorsetlayout;
	pipelinelayoutcreateinfo.pushConstantRangeCount = 0;
	pipelinelayoutcreateinfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(m_device, &pipelinelayoutcreateinfo, nullptr, &m_pipelinelayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create pipeline layout!");
	}


	VkGraphicsPipelineCreateInfo pipelinecreateinfo{};
	pipelinecreateinfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelinecreateinfo.stageCount = 2;
	pipelinecreateinfo.pStages = shaderstages;
	
	
	pipelinecreateinfo.pVertexInputState = &inputvertexcreateinfo;
	pipelinecreateinfo.pInputAssemblyState = &inputassemblycreateinfo;
	pipelinecreateinfo.pViewportState = &viewportcreateinfo;
	pipelinecreateinfo.pRasterizationState = &rastercreateinfo;
	pipelinecreateinfo.pMultisampleState = &multisamplecreateinfo;
	pipelinecreateinfo.pColorBlendState = &colorblendcreateinfo;
	pipelinecreateinfo.pDepthStencilState = nullptr;
	
	
	pipelinecreateinfo.layout = m_pipelinelayout;
	pipelinecreateinfo.pDynamicState = &dynamicstatecreateinfo;
	pipelinecreateinfo.renderPass = m_renderpass;
	pipelinecreateinfo.subpass = 0;

	pipelinecreateinfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelinecreateinfo.basePipelineIndex = -1;


	if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelinecreateinfo, nullptr, &m_pipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create the graphics pipeline");

	}




	vkDestroyShaderModule(m_device, vertexmodule, nullptr);
	vkDestroyShaderModule(m_device, fragmentmodule, nullptr);


}

void Application::CreateFramebuffers()
{
	m_swapchainframebuffers.resize(m_swapchainimageviews.size());


	for (int i = 0; i < m_swapchainimageviews.size(); i++)
	{

		VkImageView attachments[] = { m_swapchainimageviews[i] };

		VkFramebufferCreateInfo framebufferinfo{};
		framebufferinfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferinfo.renderPass = m_renderpass;
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

void Application::CreateCommandPools()
{
	
	QueueFamilyIndices indices = FindQueueFamilies(m_physicaldevice);


	VkCommandPoolCreateInfo graphicspoolinfo{};
	graphicspoolinfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	graphicspoolinfo.queueFamilyIndex = indices.graphicsfamily.value();
	graphicspoolinfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(m_device, &graphicspoolinfo,  nullptr, &m_graphicscommandpool) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create the graphics command pool!");

	}
	
	VkCommandPoolCreateInfo transferpoolinfo{};
	transferpoolinfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	transferpoolinfo.queueFamilyIndex = indices.transferfamily.value();
	transferpoolinfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(m_device, &transferpoolinfo, nullptr, &m_transfercommandpool) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create the transfer command pool!");

	}









}

void Application::CreateUniformBuffer()
{
	m_uniformbuffers.resize(MAXFRAMESINFLIGHT);
	m_uniformbuffermem.resize(MAXFRAMESINFLIGHT);
	m_uniformbuffersmapped.resize(MAXFRAMESINFLIGHT);

	VkDeviceSize buffersize = sizeof(ModelViewProjectionBuffer);

	for (int i = 0; i < MAXFRAMESINFLIGHT; i++)
	{

		CreateBuffer(buffersize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_uniformbuffers[i], m_uniformbuffermem[i], VK_SHARING_MODE_CONCURRENT);

		vkMapMemory(m_device, m_uniformbuffermem[i], 0, buffersize, 0, &m_uniformbuffersmapped[i]);

	}



}

void Application::CreateDescriptorPool()
{
	VkDescriptorPoolSize poolsize{};
	poolsize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolsize.descriptorCount = static_cast<uint32_t>(MAXFRAMESINFLIGHT);

	VkDescriptorPoolCreateInfo poolinfo{};
	poolinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolinfo.poolSizeCount = 1;
	poolinfo.pPoolSizes = &poolsize;
	poolinfo.maxSets = static_cast<uint32_t>(MAXFRAMESINFLIGHT);

	if (vkCreateDescriptorPool(m_device, &poolinfo, nullptr, &m_descriptorpool) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create descriptor pool!");

	}

}

void Application::CreateVertexBuffers()
{


	VkDeviceSize buffersize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingbuffer;
	VkDeviceMemory stagingbuffermem;

	CreateBuffer(buffersize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingbuffer, stagingbuffermem, VK_SHARING_MODE_CONCURRENT);
	

	void* data;

	vkMapMemory(m_device, stagingbuffermem, 0, buffersize, 0, &data);

	memcpy(data, vertices.data(), (size_t)buffersize);

	vkUnmapMemory(m_device, stagingbuffermem);

	CreateBuffer(buffersize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertexbuffer, m_vertexbuffermemory, VK_SHARING_MODE_CONCURRENT);

	CopyBuffer(stagingbuffer, m_vertexbuffer, buffersize, m_transfercommandpool, m_transferqueue);

	vkDestroyBuffer(m_device, stagingbuffer, nullptr);
	vkFreeMemory(m_device, stagingbuffermem, nullptr);

}

void Application::CreateIndexBuffers()
{

	VkDeviceSize buffersize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingbuffer;
	VkDeviceMemory stagingbuffermem;

	CreateBuffer(buffersize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingbuffer, stagingbuffermem, VK_SHARING_MODE_CONCURRENT);


	void* data;

	vkMapMemory(m_device, stagingbuffermem, 0, buffersize, 0, &data);

	memcpy(data, indices.data(), (size_t)buffersize);

	vkUnmapMemory(m_device, stagingbuffermem);

	CreateBuffer(buffersize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indexbuffer, m_indexbuffermemory, VK_SHARING_MODE_CONCURRENT);

	CopyBuffer(stagingbuffer, m_indexbuffer, buffersize, m_transfercommandpool, m_transferqueue);

	vkDestroyBuffer(m_device, stagingbuffer, nullptr);
	vkFreeMemory(m_device, stagingbuffermem, nullptr);

}

void Application::CreateCommandBuffer()
{
	m_commandbuffers.resize(MAXFRAMESINFLIGHT);

	VkCommandBufferAllocateInfo cmdbufferinfo{};
	cmdbufferinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdbufferinfo.commandBufferCount = static_cast<uint32_t>( m_commandbuffers.size());
	cmdbufferinfo.commandPool = m_graphicscommandpool;
	cmdbufferinfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	if (vkAllocateCommandBuffers(m_device, &cmdbufferinfo, m_commandbuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create command buffer");


	}
	


}

void Application::CreateSyncObjects()
{

	VkSemaphoreCreateInfo semaphorecreateinfo{};
	semaphorecreateinfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;


	VkFenceCreateInfo fencecreateinfo{};

	fencecreateinfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fencecreateinfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	s_imageavailable.resize(MAXFRAMESINFLIGHT);
	s_renderfinished.resize(MAXFRAMESINFLIGHT);
	f_inflightfence.resize(MAXFRAMESINFLIGHT);




	for (int i = 0; i < MAXFRAMESINFLIGHT; i++)
	{
		if (vkCreateSemaphore(m_device, &semaphorecreateinfo, nullptr, &s_imageavailable[i]) != VK_SUCCESS || vkCreateSemaphore(m_device, &semaphorecreateinfo, nullptr, &s_renderfinished[i]) != VK_SUCCESS || vkCreateFence(m_device, &fencecreateinfo, nullptr, &f_inflightfence[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Semaphores or fences could not be initialized!");
		}

	}

	

}

void Application::CleanUpSwapchain()
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

void Application::CreateTextureImage()
{
	int width, height, bpp;

	stbi_uc* pixels = stbi_load("res/Textures/Placeholder.png", &width, &height, &bpp, STBI_rgb_alpha);
	VkDeviceSize buffersize = width * height * 4;

	if (!pixels)
	{
		throw std::runtime_error("Didnt find the texture!");
	}

	VkBuffer stagingbuffer;
	VkDeviceMemory stagingmem;

	CreateBuffer(buffersize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingbuffer, stagingmem, VK_SHARING_MODE_CONCURRENT);
	
	void* data;
	vkMapMemory(m_device, stagingmem, 0, buffersize, 0, &data);
	memcpy(data, pixels, static_cast<uint32_t>(buffersize));
	vkUnmapMemory(m_device, stagingmem);

	stbi_image_free(pixels);

	CreateImage(width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, 
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_textureimage, m_textureimagemem, VK_SHARING_MODE_CONCURRENT);


	TransitionImageLayout(m_textureimage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_transfercommandpool, m_transferqueue);

	CopyBuffertoImage(stagingbuffer, m_textureimage, static_cast<uint32_t>(width), static_cast<uint32_t>(height), m_transfercommandpool, m_transferqueue);

	TransitionImageLayout(m_textureimage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_transfercommandpool, m_transferqueue);

	vkDestroyBuffer(m_device, stagingbuffer, nullptr);
	vkFreeMemory(m_device, stagingmem, nullptr);




}

void Application::CreateTextureImageView()
{

	m_teximageview = CreateImageView(m_textureimage, VK_FORMAT_R8G8B8A8_SRGB);





}

void Application::CreateTextureSampler()
{

	VkPhysicalDeviceProperties properties{};

	vkGetPhysicalDeviceProperties(m_physicaldevice, &properties);



	VkSamplerCreateInfo samplerinfo{};
	samplerinfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

	samplerinfo.magFilter = VK_FILTER_LINEAR;
	samplerinfo.minFilter = VK_FILTER_LINEAR;

	samplerinfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerinfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerinfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

	samplerinfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

	samplerinfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerinfo.mipLodBias = 0.0f;
	samplerinfo.minLod = 0.0f;
	samplerinfo.maxLod = 0.0f;

	samplerinfo.anisotropyEnable = VK_TRUE;
	samplerinfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	


	samplerinfo.unnormalizedCoordinates = VK_FALSE;
	samplerinfo.compareEnable = VK_FALSE;
	samplerinfo.compareOp = VK_COMPARE_OP_ALWAYS;

	if (vkCreateSampler(m_device, &samplerinfo, nullptr, &m_texsampler) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create sampler");

	}

		

}

bool Application::CheckValidationLayers()
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

std::vector<const char*> Application::GetRequiredInstanceExtensions()
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

VKAPI_ATTR VkBool32 VKAPI_CALL Application::DebugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{

	std::cerr<< "Validation layer: " << pCallbackData->pMessage << std::endl;


	return VK_FALSE;
}

void Application::SetDebugCreateInfoStructVariables(VkDebugUtilsMessengerCreateInfoEXT& createinfo)
{
	createinfo = {};
	createinfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createinfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createinfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createinfo.pfnUserCallback = DebugCallBack;
	createinfo.pUserData = nullptr;



}

void Application::SelectPhysicalDevice()
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

		
		/*uint32_t devicescore = RateDevice(physicaldevices[i]);
		
		if (devicescore > 0 && devicescore > bestscore)
		{
			bestscore = devicescore;
			
			m_physicaldevice = physicaldevices[i];

			
		}*/



		/*m_queuefamilyindices = FindQueueFamilies(m_physicaldevice);*/


		if (m_physicaldevice == nullptr)
		{
			throw std::runtime_error("Failed to find GPU");

		}

	}







}

bool Application::RateDevice(VkPhysicalDevice& physicaldevice, uint32_t& scorehandle, bool& presentfamily, VkPhysicalDeviceProperties* propertieshandle)
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

	if (indices.graphicsfamily.has_value())
	{
		score += 100;


	}

	if (indices.presentfamily.has_value())
	{
		score += 100;
		presentfamily = true;
	}

	if (indices.computefamily.has_value())
	{
		score += 100;


	}

	if (indices.transferfamily.has_value())
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

QueueFamilyIndices Application::FindQueueFamilies(VkPhysicalDevice& physicaldevice)
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


		// well a switch statement would be ideal honestly

		//switch (queueflag)// & VK_QUEUE_GRAPHICS_BIT)
		//{
		// case (const char)queueflag & VK_QUEUE_GRAPHICS_BIT:
		//	indices.graphicsfamily = i;
		//	break; 
		// case VK_QUEUE_COMPUTE_BIT:
		//	 indices.computefamily = i;
		//	 break;
		// case VK_QUEUE_TRANSFER_BIT:
		//	 indices.transferfamily = i;
		//	 break;

		//}


	}


	return indices;
}

bool Application::DeviceExtensionSupport(VkPhysicalDevice& physicaldevice)
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

void Application::CreateLogicalDevice()
{


	
	

	std::vector<VkDeviceQueueCreateInfo> queuecreateinfos;

	std::set<uint32_t>uniquequeuefamilies =
	{
		m_queuefamilyindices.graphicsfamily.value(),
		m_queuefamilyindices.presentfamily.value(),
		m_queuefamilyindices.transferfamily.value()

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

	devicecreateinfo.queueCreateInfoCount = queuecreateinfos.size();
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

	vkGetDeviceQueue(m_device, m_queuefamilyindices.graphicsfamily.value(), 0, &m_graphicsqueue);
	vkGetDeviceQueue(m_device, m_queuefamilyindices.presentfamily.value(), 0, &m_presentqueue);
	vkGetDeviceQueue(m_device, m_queuefamilyindices.transferfamily.value(), 0, &m_transferqueue);

}

void Application::CreateSurface()
{
	if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface!");

	}

}

SwapChainSupportDetails Application::QuerySwapChainSupport(VkPhysicalDevice& physicaldevice)
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

VkSurfaceFormatKHR Application::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableformats)
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

VkPresentModeKHR Application::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablepresentmodes)
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

VkExtent2D Application::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfacecapabilities)
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

std::vector<char> Application::ReadFile(const std::string& filepath)
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

VkShaderModule Application::CreateShaderModule(const std::vector<char>& shaderfile)
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

VkCommandBuffer Application::BeginSingleTimeCommands(VkCommandPool& commandpool, const VkCommandBufferLevel& level)
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

void Application::EndSingleTimeCommands(VkCommandBuffer& commandbuffer, const VkCommandPool& commandpool, const VkQueue& submitqueue)
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

void Application::RecordCommandBuffer(VkCommandBuffer& commandbuffer, const uint32_t& swapchainimageindex)
{

	VkCommandBufferBeginInfo cmdbufferbegininfo{};
	cmdbufferbegininfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdbufferbegininfo.flags = 0;
	cmdbufferbegininfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(commandbuffer, &cmdbufferbegininfo) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to begin command buffer!");

	}

	VkRenderPassBeginInfo renderpassbegininfo{};
	renderpassbegininfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderpassbegininfo.framebuffer = m_swapchainframebuffers[swapchainimageindex];
	renderpassbegininfo.renderPass = m_renderpass;

	renderpassbegininfo.renderArea.offset = { 0,0 };
	renderpassbegininfo.renderArea.extent = m_swapchainextent;

	VkClearValue clearcolor = { {{0.f, 0.f, 0.f, 1.0f}} };
	renderpassbegininfo.clearValueCount = 1;
	renderpassbegininfo.pClearValues = &clearcolor;


	vkCmdBeginRenderPass(commandbuffer, &renderpassbegininfo, VK_SUBPASS_CONTENTS_INLINE);


	vkCmdBindPipeline(commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

	VkBuffer buffers[] = {m_vertexbuffer};
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandbuffer, 0, 1, buffers, offsets);
	

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

	vkCmdBindIndexBuffer(commandbuffer, m_indexbuffer, 0, VK_INDEX_TYPE_UINT16);

	vkCmdBindDescriptorSets(commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelinelayout, 0, 1, &m_descriptorsets[m_currentframe], 0, nullptr);

	vkCmdDrawIndexed(commandbuffer, static_cast<uint32_t> (indices.size()), 1, 0, 0, 0);


	vkCmdEndRenderPass(commandbuffer);

	if (vkEndCommandBuffer(commandbuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to record command buffer");

	}
	






}

void Application::DrawFrame()
{

	vkWaitForFences(m_device, 1, &f_inflightfence[m_currentframe], VK_TRUE, UINT64_MAX);
	


	uint32_t imageindex;
	VkResult result = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, s_imageavailable[m_currentframe], f_inflightfence[m_currentframe], &imageindex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapchain();
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to aquire swapchain image!");

	}
	
	vkResetFences(m_device, 1, &f_inflightfence[m_currentframe]);
	


	UpdateUniformBuffer(m_currentframe);


	vkResetCommandBuffer(m_commandbuffers[m_currentframe], 0);

	RecordCommandBuffer(m_commandbuffers[m_currentframe], imageindex);

	VkSemaphore waitsemaphores[] = {s_imageavailable[m_currentframe]};
	VkSemaphore signalsemaphores[] = {s_renderfinished[m_currentframe]};
	VkPipelineStageFlags waitstages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

	VkSubmitInfo submitinfo{};
	

	submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitinfo.commandBufferCount = 1;
	submitinfo.pCommandBuffers = &m_commandbuffers[m_currentframe];
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
	presentinfo.pWaitSemaphores = waitsemaphores;

	presentinfo.swapchainCount = 1;
	presentinfo.pSwapchains = swapchains;

	presentinfo.pImageIndices = &imageindex;

	presentinfo.pResults = nullptr;

	result = vkQueuePresentKHR(m_presentqueue, &presentinfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result != VK_SUBOPTIMAL_KHR || m_windowresized)
	{
		RecreateSwapchain();
		m_windowresized = false;

	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to present queue!");

	}


	m_currentframe = (m_currentframe + 1) % MAXFRAMESINFLIGHT;

}

void Application::RecreateSwapchain()
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
	CreateFramebuffers();





	
}

void Application::CreateBuffer(const VkDeviceSize& size, VkBufferUsageFlags usageflags, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& buffermemory, VkSharingMode sharingmode)
{
	VkBufferCreateInfo buffercreateinfo{};
	buffercreateinfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffercreateinfo.size = size;

	buffercreateinfo.usage = usageflags;
	buffercreateinfo.sharingMode = sharingmode;

	if (vkCreateBuffer(m_device, &buffercreateinfo, nullptr, &buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create vertex buffer!");

	}

	VkMemoryRequirements memrequirements;
	vkGetBufferMemoryRequirements(m_device, buffer, &memrequirements);


	VkMemoryAllocateInfo allocateinfo{};
	allocateinfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateinfo.allocationSize = memrequirements.size;
	allocateinfo.memoryTypeIndex = FindMemoryType(memrequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(m_device, &allocateinfo, nullptr, &buffermemory) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate vertex buffer memory!");
	}


	vkBindBufferMemory(m_device, buffer, buffermemory, 0);


}

void Application::CreateImage(const uint32_t& width, const uint32_t height, VkFormat format, VkImageTiling tiling, const VkImageUsageFlags& usage, const VkMemoryPropertyFlags& properties, VkImage& image, VkDeviceMemory& imagememory, VkSharingMode sharingmode)
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
	imageinfo.flags = 0;

	if (vkCreateImage(m_device, &imageinfo, nullptr, &image) != VK_SUCCESS)
	{
		throw std::runtime_error("There is always gonna be someone who's better than you at something, that doesnt make you any less. I AM THAT I AM, I want to see who dares oppose that! also your image couldnt be created :( !");

	}

	VkMemoryRequirements imagememrequirements;
	vkGetImageMemoryRequirements(m_device, image, &imagememrequirements);

	VkMemoryAllocateInfo allocinfo{};
	allocinfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocinfo.allocationSize = imagememrequirements.size;
	allocinfo.memoryTypeIndex = FindMemoryType(imagememrequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(m_device, &allocinfo, nullptr, &imagememory) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate texture memory.");
	}

	vkBindImageMemory(m_device, image, imagememory, 0);



}

void Application::CopyBuffer(VkBuffer& srcbuffer, VkBuffer& dstbuffer, VkDeviceSize size, VkCommandPool& commandpool, VkQueue& submitqueue)
{
	
	VkCommandBuffer commandbuffer = BeginSingleTimeCommands(commandpool);


	VkBufferCopy copyregion{};
	copyregion.srcOffset = 0;
	copyregion.dstOffset = 0;
	copyregion.size = size;
	vkCmdCopyBuffer(commandbuffer, srcbuffer, dstbuffer, 1, &copyregion);

	EndSingleTimeCommands(commandbuffer, commandpool, submitqueue);


}

uint32_t Application::FindMemoryType(uint32_t typefilter, VkMemoryPropertyFlags flags)
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

void Application::UpdateUniformBuffer(const uint32_t& currentframe)
{
	static auto starttime = std::chrono::high_resolution_clock::now();
	
	auto currenttime = std::chrono::high_resolution_clock::now();
	
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currenttime - starttime).count();

	ModelViewProjectionBuffer mvp;

	mvp.model = glm::mat4(1.0);
	mvp.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0f, 0.0f, 1.0f));
	mvp.projection = glm::perspective(45.f, ((float)m_swapchainextent.width / (float)m_swapchainextent.height), 0.1f, 100.f);

	mvp.projection[1][1] *= -1;

	memcpy(m_uniformbuffersmapped[currentframe], &mvp, sizeof(mvp));


}

void Application::TransitionImageLayout(VkImage& image, const VkFormat& format, VkImageLayout oldlayout, VkImageLayout newlayout, VkCommandPool& commandpool, VkQueue submitqueue)
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



	


	vkCmdPipelineBarrier(commandbuffer,
		srcstage, //todo
		dststage, //todo
		0, 0,
		nullptr, 0,
		nullptr, 1,
		&membarrier);

	EndSingleTimeCommands(commandbuffer, commandpool, submitqueue);


}

void Application::CopyBuffertoImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height, VkCommandPool& commandpool, VkQueue& submitqueue)
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

VkImageView Application::CreateImageView(VkImage& image, VkFormat format, VkImageAspectFlags imageaspect)
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






//bool Application::IsDeviceSuitable(VkPhysicalDevice& physicaldevice)
//{
//	VkPhysicalDeviceProperties properties;
//	vkGetPhysicalDeviceProperties(physicaldevice, &properties);
//
//	VkPhysicalDeviceFeatures features;
//	vkGetPhysicalDeviceFeatures(physicaldevice, &features);
//
//	return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && features.geometryShader;
//}




