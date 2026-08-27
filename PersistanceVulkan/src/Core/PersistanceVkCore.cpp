/*****************************************************************//**
 * @file   PersistanceVkCore.cpp
 * @brief  This is the implementation file for the functions in PersistanceVkCore.h
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-29-2026
 * @cite   The Vulkan Documentation: https://docs.vulkan.org/spec/latest/index.html
 *********************************************************************/

#include "./PersistanceVkCore.h"

#include "./CoreUtils.h"
#include "../Backend/RenderPass.h"
#include "../Backend/RenderPassAttachment.h"


#define STB_IMAGE_IMPLEMENTATION
#include "../Vendor/stb_image.h"
#include "PersistanceVkCore.h"


#define VMA_IMPLEMENTATION


#define VOLK_IMPLEMENTATION

uint32_t PersistanceVkCore::getScreenWidth()
{
    return m_screenWidth;
}

uint32_t PersistanceVkCore::getScreenHeight()
{
    return m_screenHeight;
}

const VmaAllocator &PersistanceVkCore::getAllocator()
{
    return m_vmaAllocator;
}

/**
 * @brief Adds an extension to the Vulkan Logical Device
 * 
 * @param extensionName 
 */
void PersistanceVkCore::addExtension(const char *extensionName)
{
	m_deviceextensions.push_back(extensionName);
}

/**
 * @brief Initializes a GLFW window
 *
 */
void PersistanceVkCore::initWindow(const uint32_t screenWidth, const uint32_t screenHeight)
{
	if (m_window == nullptr)
	{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_window = glfwCreateWindow(screenWidth, screenHeight, "Vulkan window", nullptr, nullptr);
		
		std::cout << "Initialized Window \n";
		
		glfwSetWindowUserPointer(m_window, this);
		
		glfwSetWindowSizeCallback(m_window, resizeWindowCallback);
		
		m_screenWidth = screenWidth;
		m_screenHeight = screenHeight;
	}
		
}
/**
 * @brief Initializes a Vulkan instance, surface, device, memory allocator, and sync objects
 * 
 */
void PersistanceVkCore::initVulkan(const void* deviceFeatures)
{
	volkInitialize();
	createInstance();
	setupDebugCallBack();
	volkLoadInstance(m_instance);
	createSurface();
	selectPhysicalDevice();
	createLogicalDevice(deviceFeatures);
	volkLoadDevice(m_device);
	createAllocator();
	createSwapChain();
	createCommandPools();
	createSyncObjects();
	
}

/**
 * @brief Creates a vulkan instance.
 * 
 */
void PersistanceVkCore::createInstance()
{
	if (enablevalidationlayers && !checkValidationLayers())
	{
		std::cout << "Tried Getting Validation Layers and Failed. \n";
		BREAK;
	}

	//application info struct
	VkApplicationInfo appinfo{};
	appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appinfo.pApplicationName = "Engine";
	appinfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
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

		setupDebugConfigCreateInfo(debugcreateinfo);
		createinfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugcreateinfo;


	}
	else
	{
		createinfo.enabledLayerCount = 0;
		createinfo.pNext = nullptr;
	}

	createinfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

	std::vector<const char*> requiredextensions = getRequiredExtensions();


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
		std::cout << "PersistanceVk Name: " << appinfo.pApplicationName << "\nEngine: " << appinfo.pEngineName << "\n";


	}
}

/**
 * @brief Creates a Vulkan surface with the GLFW window.
 * 
 */
void PersistanceVkCore::createSurface()
{
	if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface!");

	}

}


/**
 * @brief Selects the physical device with the most supported necessary features.
 * 
 */
void PersistanceVkCore::selectPhysicalDevice()
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
		QueueFamilyIndices indices = findQueueFamilies(physicaldevices[i]);

		bool haspresentfamilyindex;

		VkPhysicalDeviceProperties deviceproperties;

		if (rateDevice(physicaldevices[i], score, haspresentfamilyindex, &deviceproperties))
		{

			if (score > bestscore)

			{
				bestscore = score;

				m_physicalDevice = physicaldevices[i];
				m_queueFamilyIndices = indices;

				if (enablevalidationlayers)
				{
					std::cout << "\n  Using Device: " << deviceproperties.deviceName << "\n \n";

				}


			}


		}


		
	}
	
	if (m_physicalDevice == nullptr)
	{
		throw std::runtime_error("Failed to find GPU");

	}





}

/**
 * @brief Creates a Vulkan logical device for the selected physical device.
 * 
 */
void PersistanceVkCore::createLogicalDevice(const void* deviceFeatures)
{
	std::vector<VkDeviceQueueCreateInfo> queuecreateinfos;

	std::set<uint32_t>uniquequeuefamilies =
	{
		m_queueFamilyIndices.graphicsfamily,
		m_queueFamilyIndices.presentfamily,
		m_queueFamilyIndices.transferfamily,
		m_queueFamilyIndices.computefamily

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


	VkPhysicalDeviceFeatures phyisicalDeviceFeatures{};
	phyisicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
	
	VkDeviceCreateInfo devicecreateinfo{};

	devicecreateinfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	devicecreateinfo.pNext = deviceFeatures;

	devicecreateinfo.queueCreateInfoCount = static_cast<uint32_t>(queuecreateinfos.size());
	devicecreateinfo.pQueueCreateInfos = queuecreateinfos.data();


	devicecreateinfo.pEnabledFeatures = &phyisicalDeviceFeatures;
	

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



	if (vkCreateDevice(m_physicalDevice, &devicecreateinfo, nullptr, &m_device) != VK_SUCCESS)
	{
		throw std::runtime_error("Could not create logical device");

	}

	vkGetDeviceQueue(m_device, m_queueFamilyIndices.graphicsfamily, 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_device, m_queueFamilyIndices.presentfamily, 0, &m_presentQueue);
	vkGetDeviceQueue(m_device, m_queueFamilyIndices.transferfamily, 0, &m_transferQueue);
	vkGetDeviceQueue(m_device, m_queueFamilyIndices.computefamily, 0, &m_computeQueue);

}

/**
 * @brief Creates a Vulkan Memory Allocator through the VMA library.
 * 
 */
void PersistanceVkCore::createAllocator()
{
	m_vmaAllocCreateInfo.device = m_device;
	m_vmaAllocCreateInfo.physicalDevice = m_physicalDevice;
	m_vmaAllocCreateInfo.instance = m_instance;
	m_vmaAllocCreateInfo.vulkanApiVersion = VK_API_VERSION_1_0;
	m_vmaAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;

	m_vmaFunctions = {};
	m_vmaFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
	m_vmaFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

	m_vmaAllocCreateInfo.pVulkanFunctions = &m_vmaFunctions;

	if (vmaCreateAllocator(&m_vmaAllocCreateInfo, &m_vmaAllocator) != VK_SUCCESS)
	{
		BREAK(0);
	}

}

/**
 * @brief Creates the swapchain and its images.
 * 
 */
void PersistanceVkCore::createSwapChain()
{
	SwapChainSupportDetails details = querySwapchainSupportDetails(m_physicalDevice);

	VkSurfaceFormatKHR surfaceformat = chooseSwapSurfaceFormat(details.surfaceformat);
	VkPresentModeKHR surfacepresentmode = chooseSwapPresentMode(details.presentmode);
	VkExtent2D extent = chooseSwapExtent(details.surfacecapabilities);

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


	QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

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

	m_swapchainFramebuffers.images.resize(swapchainimagecount);

	vkGetSwapchainImagesKHR(m_device, m_swapchain, &swapchainimagecount, m_swapchainFramebuffers.images.data());

	m_swapchainImageFormat = surfaceformat.format;

	m_swapchainExtent = extent;


}

/**
 * @brief Creates the Vulkan Command Pools.
 * 
 */
void PersistanceVkCore::createCommandPools()
{
	QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

	if (indices.graphicsfamily != -1)
	{
		VkCommandPoolCreateInfo graphicspoolinfo{};
		graphicspoolinfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		graphicspoolinfo.queueFamilyIndex = indices.graphicsfamily;
		graphicspoolinfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if (vkCreateCommandPool(m_device, &graphicspoolinfo, nullptr, &m_graphicsCommandPool) != VK_SUCCESS)
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

		if (vkCreateCommandPool(m_device, &transferpoolinfo, nullptr, &m_transferCommandPool) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create the transfer command pool!");
		}

	}

	if (indices.computefamily != -1) 
	{
		VkCommandPoolCreateInfo computepoolinfo{};
		computepoolinfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		computepoolinfo.queueFamilyIndex = indices.computefamily;
		computepoolinfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if (vkCreateCommandPool(m_device, &computepoolinfo, nullptr, &m_computeCommandPool) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create the compute command pool");
		}

	}

}

/**
 * @brief Creates sync objects like Semaphores and Fences.
 * 
 */
void PersistanceVkCore::createSyncObjects()
{
	VkSemaphoreCreateInfo semaphorecreateinfo{};
	semaphorecreateinfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fencecreateinfo{};

	fencecreateinfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fencecreateinfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	s_imageAvailable.resize(PersistanceLib::MAXFRAMESINFLIGHT);
	s_renderFinished.resize(m_swapchainFramebuffers.images.size());
	f_inFlightFence.resize(PersistanceLib::MAXFRAMESINFLIGHT);
	


	for (int i = 0; i < PersistanceLib::MAXFRAMESINFLIGHT; i++)
	{
		if (vkCreateSemaphore(m_device, &semaphorecreateinfo, nullptr, &s_imageAvailable[i]) != VK_SUCCESS  
			|| vkCreateFence(m_device, &fencecreateinfo, nullptr, &f_inFlightFence[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Semaphores or fences could not be initialized!");
		}

	}
	for (auto& renderFinished : s_renderFinished) {
		if (vkCreateSemaphore(m_device, &semaphorecreateinfo, nullptr, &renderFinished) != VK_SUCCESS) 
		{
			throw std::runtime_error("Semaphores or fences could not be initialized!");
		}
	}
}

/**
 * @brief Creates the main renderpass.
 * 
 * @param doDepthTesting Will the main renderpass require depth testing.
 */
void PersistanceVkCore::createSwapchainRenderPass()
{

	RenderPassAttachment colorattachment = PersistanceBackend::createRenderPassAttachment(
		0, 
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 
		m_swapchainImageFormat, 
		VK_SAMPLE_COUNT_1_BIT, 
		VK_ATTACHMENT_LOAD_OP_CLEAR, 
		VK_ATTACHMENT_STORE_OP_STORE, 
		VK_IMAGE_LAYOUT_UNDEFINED, 
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	);
	RenderPassAttachment depthAttachment = PersistanceBackend::createRenderPassAttachment(
		1, 
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 
		PersistanceUtils::findDepthFormat(),
		VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	);
	

	AttachmentReferenceList refList;
	refList.add(&colorattachment, 1);
	AttachmentDescriptionList desList;
	desList.add(&colorattachment, 1);

	if (m_depthTesting) 
	{
		desList.add(&depthAttachment, 1);
	}
	
	RenderPassAttachment* pDepthAttachment = m_depthTesting ? &depthAttachment : nullptr;

	VkPipelineStageFlags srcStageFlags = m_depthTesting ? VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT : VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkPipelineStageFlags dstStageFlags = m_depthTesting ? VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT : VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkAccessFlags srcAccessFlags = m_depthTesting ? VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT : 0;
	VkAccessFlags dstAccessFlags = m_depthTesting ? VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT : VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkSubpassDescription description = PersistanceBackend::createSubpassDescription(&refList, pDepthAttachment, nullptr, nullptr, 0);
	VkSubpassDependency dependency = PersistanceBackend::createSubpassDependency(VK_SUBPASS_EXTERNAL, 0, srcAccessFlags, dstAccessFlags, srcStageFlags, dstStageFlags, 0);

	m_mainRenderPass = PersistanceBackend::createRenderPass(&description, 1, &dependency, 1, desList);

}

/**
 * @brief creates the swapchain image views
 * 
 */
void PersistanceVkCore::createSwapchainImageViews()
{
	size_t size = m_swapchainFramebuffers.images.size();

	m_swapchainFramebuffers.imageviews.resize(size);

	for (uint32_t i = 0; i < size; i++) 
	{
		m_swapchainFramebuffers.imageviews[i] = PersistanceUtils::createImageView(m_swapchainFramebuffers.images[i], m_swapchainImageFormat);

	}
	
}


/**
 * @brief Creates the swapchain framebuffers\.
 * 
 */
void PersistanceVkCore::createSwapchainFramebuffers(VkImageView* depthBufferImageView)
{
	size_t imageViewCount = m_swapchainFramebuffers.imageviews.size();
	m_swapchainFramebuffers.framebuffers.resize(imageViewCount);


	for (uint32_t i = 0; i < imageViewCount; i++)
	{
		std::vector<VkImageView> attachments = {
			m_swapchainFramebuffers.imageviews[i]
		};
		
		if (depthBufferImageView != nullptr) 
		{
			attachments.push_back(*depthBufferImageView);
		}
		

		VkFramebufferCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.height = m_swapchainExtent.height;
		info.width = m_swapchainExtent.width;
		info.layers = 1;
		info.renderPass = m_mainRenderPass;
		info.attachmentCount = static_cast<uint32_t>(attachments.size());
		info.pAttachments = attachments.data();
		info.flags = 0;

		if (vkCreateFramebuffer(m_device, &info, nullptr, &m_swapchainFramebuffers.framebuffers[i]) != VK_SUCCESS)
		{
			BREAK(1);
		}


	}

}

/**
 * @brief Creates a depth buffer image and image view for the swapchain renderpass.
 * 
 */
void PersistanceVkCore::createSwapchainDepthBuffer()
{
	VkImage& depthImage = m_swapchainDepthBuffer.image;
	VmaAllocation& depthAllocation = m_swapchainDepthBuffer.allocation;


	VkFormat format = PersistanceUtils::findDepthFormat();

	PersistanceUtils::createImage(m_swapchainExtent.width, m_swapchainExtent.height, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthAllocation, VK_SHARING_MODE_CONCURRENT, VK_IMAGE_LAYOUT_UNDEFINED);

	m_swapchainDepthBuffer.imageview = PersistanceUtils::createImageView(depthImage, format, VK_IMAGE_ASPECT_DEPTH_BIT);
}

/**
 * @brief Recreates the swapchain and framebuffers.
 * 
 */
void PersistanceVkCore::recreateSwapchain()
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

	cleanUpSwapchain();

	createSwapChain();
	createSwapchainImageViews();
	createSwapchainDepthBuffer();

	VkImageView* depthBufferImageView = m_depthTesting ? &m_swapchainDepthBuffer.imageview : nullptr;
	createSwapchainFramebuffers(depthBufferImageView);


}

/**
 * @brief Cleans up the swapchain resources i.e. the framebuffers and imageviews.
 *
 */
void PersistanceVkCore::cleanUpSwapchain()
{

	for (auto framebuffer : m_swapchainFramebuffers.framebuffers)
	{
		vkDestroyFramebuffer(m_device, framebuffer, nullptr);
	}
	for (const auto& imageviews : m_swapchainFramebuffers.imageviews)
	{
		vkDestroyImageView(m_device, imageviews, nullptr);
	}

	
	
	vmaDestroyImage(m_vmaAllocator, m_swapchainDepthBuffer.image, m_swapchainDepthBuffer.allocation);
	vkDestroyImageView(m_device, m_swapchainDepthBuffer.imageview, nullptr);
	

	vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

}
/**
 * @brief Destroys the main renderpass resource.
 * 
 */
void PersistanceVkCore::cleanUpMainRenderPass()
{
	vkDestroyRenderPass(m_device, m_mainRenderPass, nullptr);

}
void PersistanceVkCore::cleanUpSyncObjects()
{
	for (int i = 0; i < PersistanceLib::MAXFRAMESINFLIGHT; i++)
	{
		vkDestroySemaphore(m_device, s_imageAvailable[i], nullptr);
		vkDestroyFence(m_device, f_inFlightFence[i], nullptr);
		
	}
	for (auto& semaphore : s_renderFinished) 
	{
		vkDestroySemaphore(m_device, semaphore, nullptr);

	}
}
/**
 * @brief Cleans up command pools.
 * 
 */
void PersistanceVkCore::cleanUpCommandPools()
{
	vkDestroyCommandPool(m_device, m_graphicsCommandPool, nullptr);
	vkDestroyCommandPool(m_device, m_transferCommandPool, nullptr);
	vkDestroyCommandPool(m_device, m_computeCommandPool, nullptr);

}

/**
 * @brief Cleans up the VMA memory allocator.
 * 
 */
void PersistanceVkCore::cleanUpAllocator()
{
	vmaDestroyAllocator(m_vmaAllocator);
}

/**
 * @brief Creates the swapchain render pass, imageviews and framebuffers with the specified settings.
 * 
 * @param doDepthTesting Is depth testing needed.
 */
void PersistanceVkCore::createMainRenderSetup(bool doDepthTesting)
{
	m_depthTesting = doDepthTesting;
		
	if (m_depthTesting) {
		
		createSwapchainDepthBuffer();
	}
	VkImageView* depthBufferImageView = m_depthTesting ?  &m_swapchainDepthBuffer.imageview : nullptr;


	createSwapchainRenderPass();
	createSwapchainImageViews();
	createSwapchainFramebuffers(depthBufferImageView);


}


/**
 * @brief Begin the main renderpass on a specified command buffer.
 * 
 * @param commandBuffer the specified command buffer
 * @param imageIndex If set, the framebuffer selected will be of that index.
 */
void PersistanceVkCore::beginMainRenderPass(VkCommandBuffer &commandBuffer, const uint32_t *imageIndex)
{
	VkOffset2D offset{0,0};
	
	VkClearValue clearValues[2];

	clearValues[0].color = { 0.f, 0.f, 0.f, 1.f };
	clearValues[1].depthStencil = { 1.0f, 0};
	
	
	uint32_t clearValueCount = m_depthTesting ? 2 : 1;

	PersistanceBackend::beginRenderPass(commandBuffer, m_mainRenderPass, m_swapchainFramebuffers, offset, m_swapchainExtent, clearValues, clearValueCount, imageIndex);
}


/**
 * @brief Prepares the next swapchain image to be presented unto.
 * Manages the following:
 * -Waits for the inflight fence
 * -Acquires the next image in the swapchain
 * -Recreates the swapchain in case the image is outdated.
 * -Resets the fences
 * 
 * @param currentFrame The current frame drawn.
 * 
 */
void PersistanceVkCore:: startDrawing()
{
	// if (m_currentlyDrawing)
	// {
	// 	std::cout << "Previous drawing not ended properly";
	// }
	// else
	// {
	// 	m_currentlyDrawing = true;
	// }
	// Array of m_currentlyDrawing booleans size MAXFRAMESINFLIGHT needed.

	vkWaitForFences(m_device, 1, &f_inFlightFence[m_currentFrame], VK_TRUE, UINT64_MAX);


	VkResult result = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, s_imageAvailable[m_currentFrame], VK_NULL_HANDLE, &m_imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapchain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to aquire swapchain image!");
	}

	

	vkResetFences(m_device, 1, &f_inFlightFence[m_currentFrame]);

}

/**
 * @brief Waits for the main fences of the current frame to be free.
 * 
 */
void PersistanceVkCore::waitForCurrentFence()
{
	vkWaitForFences(m_device, 1, &f_inFlightFence[m_currentFrame], VK_TRUE, UINT64_MAX);
}

/**
 * @brief Returns the status about the acquired image.
 * 
 * @return The corresponding return status code.
 */
VkResult PersistanceVkCore::acquireNextSwapchainImage()
{
	return vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, s_imageAvailable[m_currentFrame], VK_NULL_HANDLE, &m_imageIndex);
}

/**
 * @brief Checks if the swapchain image is out of date and executes the function callback if this is true.
 * 
 * @param functions callback functions that preferably rerecord the command buffers
 * @param count the amount of callback functions.
 */
void PersistanceVkCore::reRecordCommandBuffersCallBack(std::function<void()> *functions, const uint32_t count)
{
	for (uint32_t i = 0; i < count; i++) {
		functions[i]();
	}
}

/**
 * @brief Ends the drawing process and presents the swapchain image to the screen.
 * 
 * @param commandBuffers Command buffers to be submitted.
 * @param commandBufferCount How many command buffers buffers must be submitted.
 */
void PersistanceVkCore::endDrawingandPresent(VkCommandBuffer* commandBuffers, const uint32_t commandBufferCount)
{

	VkSemaphore waitsemaphores[] = { s_imageAvailable[m_currentFrame] };
	VkSemaphore signalsemaphores[] = { s_renderFinished[m_imageIndex] };
	VkPipelineStageFlags waitstages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submitinfo{};


	submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitinfo.commandBufferCount = commandBufferCount;
	submitinfo.pCommandBuffers = commandBuffers;
	submitinfo.waitSemaphoreCount = 1;
	submitinfo.pWaitSemaphores = waitsemaphores;
	submitinfo.pWaitDstStageMask = waitstages;
	submitinfo.signalSemaphoreCount = 1;
	submitinfo.pSignalSemaphores = signalsemaphores;



	if (vkQueueSubmit(m_graphicsQueue, 1, &submitinfo, f_inFlightFence[m_currentFrame]) != VK_SUCCESS)
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

	presentinfo.pImageIndices = &m_imageIndex;

	presentinfo.pResults = nullptr;

	VkResult result = vkQueuePresentKHR(m_presentQueue, &presentinfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_windowResized)
	{
		m_windowResized = false;
		recreateSwapchain();

	}
	else if (result != VK_SUCCESS)
	{
		std::cout << "Failed to present queue!";
		BREAK;
	}

	m_currentFrame = (m_currentFrame + 1) % PersistanceLib::MAXFRAMESINFLIGHT;
}

/**
 * @brief Binds a graphics pipeline during command buffer exectution.
 * 
 * @param commandBuffer	The command buffer executed.
 * @param graphicsPipeline Bound graphics pipeline.
 */
void PersistanceVkCore::bindGraphicsPipeline(VkCommandBuffer& commandBuffer, const VkPipeline& graphicsPipeline)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

}

/**
 * @brief Draws a mesh with an index buffer.
 * 
 * @param commandBuffer The command buffer executed.
 * @param vertexBuffers An array of mesh vertex buffers.
 * @param vertexBufferCount The mesh vertex buffers count.
 * @param offsets An offset of the start of the buffer segment inside the same index bound vertex buffer in the array.
 * @param indexBuffer The mesh index buffers.
 * @param graphicsPipeline The graphics pipeline in use.
 * @param graphicsPipelineLayout The graphics pipeline layout in use.
 * @param descriptorSets The graphic pipeline descriptor sets.
 * @param descriptorSetCount The amount of bound descriptor sets.
 */
void PersistanceVkCore::drawIndexed(VkCommandBuffer& commandBuffer, const Buffer* vertexBuffers, const uint32_t vertexBufferCount, const VkDeviceSize* offsets, Buffer& indexBuffer, VkPipeline& graphicsPipeline, VkPipelineLayout& graphicsPipelineLayout, const VkDescriptorSet* descriptorSets, uint32_t descriptorSetCount)
{

	
	std::vector<VkBuffer> buffers;
	buffers.reserve(vertexBufferCount);

	for (size_t i = 0; i < vertexBufferCount; i++) {
		buffers.emplace_back(vertexBuffers[i].buffer);

	}


	vkCmdBindVertexBuffers(commandBuffer, 0, vertexBufferCount, buffers.data(), offsets);
	
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer.buffer, 0, PersistanceUtils::findIndexType(indexBuffer.elementSize));

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineLayout, 0, descriptorSetCount, descriptorSets, 0, nullptr);

	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indexBuffer.size / indexBuffer.elementSize), 1, 0, 0, 0);


}

void PersistanceVkCore::finalize()
{
	cleanUpSwapchain();
	cleanUpMainRenderPass();
	cleanUpSyncObjects();
	cleanUpCommandPools();
	cleanUpAllocator();


	vkDestroyDevice(m_device, nullptr);

	if (enablevalidationlayers)
	{
		DebugUtilsMessengerEXT::Destroy(m_instance, m_debugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

	vkDestroyInstance(m_instance, nullptr);

	glfwDestroyWindow(m_window);

	glfwTerminate();




}


/*
* 
* Checks for available validation layers.
*
* 
* \return true if all validations layers are available, false if layers have not been found
* 
*/
bool PersistanceVkCore::checkValidationLayers()
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

void PersistanceVkCore::setupDebugCallBack()
{
	if (!enablevalidationlayers)
	{
		return;
	}

	VkDebugUtilsMessengerCreateInfoEXT createinfo = VkDebugUtilsMessengerCreateInfoEXT();
	setupDebugConfigCreateInfo(createinfo);

	if (DebugUtilsMessengerEXT::Create(m_instance, &createinfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug messenger");

	}
}

/**
 * .
 * \return the required extensions for the Vulkan instance that will be created
 */

std::vector<const char*> PersistanceVkCore::getRequiredExtensions()
{//glfw extensions
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


/**
 * .
 * 
 * \param physicaldevice the physical device to find extension support from
 * \return 
 */
bool PersistanceVkCore::deviceExtensionSupport(VkPhysicalDevice& physicaldevice)
{
	uint32_t extensioncount;



	vkEnumerateDeviceExtensionProperties(physicaldevice, nullptr, &extensioncount, nullptr);

	std::vector<VkExtensionProperties> availableextensions(extensioncount);

	vkEnumerateDeviceExtensionProperties(physicaldevice, nullptr, &extensioncount, availableextensions.data());

	std::set<std::string> requiredextensions = { m_deviceextensions.begin(), m_deviceextensions.end() };

	for (const auto& extension : availableextensions)
	{
		requiredextensions.erase(extension.extensionName);


	}

	return requiredextensions.empty();
	
}


/**
 * Finds all Queue family indices for the Vulkan queues (i.e. graphics queue, transfer queue, etc..)
 * 
 * \param physicaldevice 
 * \return the family indices for the Vulkan queues
 */
QueueFamilyIndices PersistanceVkCore::findQueueFamilies(VkPhysicalDevice& physicaldevice)
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

/**
 * Rates a device's competence to be used for the Vulkan implementation.
 * 
 * \param physicaldevice
 * \param scorehandle the device's score to be returned;
 * \param presentfamily wether or not the device has a present family
 * \param propertieshandle the device's properties.
 * \return false if the device does not support geometry, the device does not support required extensions, or if the swapchain is inadequate.
 */
bool PersistanceVkCore::rateDevice(VkPhysicalDevice& physicaldevice, uint32_t& scorehandle, bool& presentfamily, VkPhysicalDeviceProperties* propertieshandle)
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





	auto indices = findQueueFamilies(physicaldevice);

	const bool& requiredextensionsupport = deviceExtensionSupport(physicaldevice);

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

		SwapChainSupportDetails swapchainsupport = querySwapchainSupportDetails(physicaldevice);

		isswapchainadequate = !swapchainsupport.surfaceformat.empty() && !swapchainsupport.presentmode.empty();


	}


	if (!isswapchainadequate)
	{
		return false;

	}


	scorehandle = score;


	return true;
}


/**
 * Queries the amount of formats that the swapchain supports.
 * 
 * \param physicaldevice the physical device
 * \return the swapchain support capabilities
 */
SwapChainSupportDetails PersistanceVkCore::querySwapchainSupportDetails(VkPhysicalDevice& physicaldevice)
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

/**
 * Chooses a swapchain format with the B8G8R8A8 SRGB format and nonlinear color space
 * or the first format if this configuration is not found
 * 
 * \param availableformats
 * \return the chosen format
 */
VkSurfaceFormatKHR PersistanceVkCore::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableformats)
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


/**
 * Chooses the swapchain present mode of VK_PRESENT_MODE_MAILBOX_KHR or
 * VK_PRESENT_MODE_FIFO_KHR if not available.
 * 
 * \param availablepresentmodes
 * \return VK_PRESENT_MODE_MAILBOX_KHR or VK_PRESENT_MODE_FIFO_KHR if not available.
 */
VkPresentModeKHR PersistanceVkCore::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablepresentmodes)
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

/**
 * Chooses the best extent for the swapchain from the surface capabilities or the window framebuffer size.
 * 
 * \param surfacecapabilities
 * \return the best possible extent.
 */
VkExtent2D PersistanceVkCore::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfacecapabilities)
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

GLFWwindow *PersistanceVkCore::getWindow()
{
    return m_window;
}

void PersistanceVkCore::setWindow(GLFWwindow *window)
{
	m_window = window;
}

bool PersistanceVkCore::windowResized()
{
    return m_windowResized;
}

const VkInstance &PersistanceVkCore::getInstance()
{
    return m_instance;
}

const VkPhysicalDevice &PersistanceVkCore::getPhysicalDevice()
{
    return m_physicalDevice;
}

QueueFamilyIndices *PersistanceVkCore::getQueueFamilyIndices()
{
    return &m_queueFamilyIndices;
}

const VkDevice &PersistanceVkCore::getDevice()
{
    return m_device;
}

VkQueue &PersistanceVkCore::getGraphicsQueue()
{
    return m_graphicsQueue;
}

VkQueue &PersistanceVkCore::getPresentQueue()
{
    return m_presentQueue;
}

VkQueue &PersistanceVkCore::getTransferQueue()
{
    return m_transferQueue;
}

VkQueue &PersistanceVkCore::getComputeQueue()
{
	return m_computeQueue;
}

const VkSwapchainKHR &PersistanceVkCore::getSwapchain()
{
    return m_swapchain;
}

const VkFormat PersistanceVkCore::getSwapchainFormat()
{
    return m_swapchainImageFormat;
}

Framebuffer *PersistanceVkCore::getSwapchainFramebuffers()
{
    return &m_swapchainFramebuffers;
}

VkRenderPass &PersistanceVkCore::getMainRenderPass()
{
	return m_mainRenderPass;
}

Texture *PersistanceVkCore::getSwapchainDepthBuffer()
{
    return &m_swapchainDepthBuffer;
}

bool PersistanceVkCore::swapchainIsDepthTesting()
{
    return m_depthTesting;
}

VkExtent2D PersistanceVkCore::getSwapchainExtent()
{
    return m_swapchainExtent;
}

VkCommandPool &PersistanceVkCore::getGraphicsCommandPool()
{
    return m_graphicsCommandPool;
}

VkCommandPool &PersistanceVkCore::getTransferCommandPool()
{
    return m_transferCommandPool;
}

VkCommandPool &PersistanceVkCore::getComputeCommandPool()
{
	return m_computeCommandPool;
}

uint32_t PersistanceVkCore::getCurrentFrame()
{
    return m_currentFrame;
}

uint32_t PersistanceVkCore::getImageIndex()
{
    return m_imageIndex;
}
