#include "PersistanceVkCore.h"

#include "./CoreUtils.h"
#include "../Backend/RenderPass.h";
#include "../Backend/RenderPassAttachment.h"

/**
 * Initiates a GLFW window.
 * 
 */
void PersistanceVkCore::initWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);


	m_window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

	std::cout << "Initialized Window \n";

	glfwSetWindowUserPointer(m_window, this);

	glfwSetWindowSizeCallback(m_window, resizeWindowCallback);

}

void PersistanceVkCore::initVulkan()
{
	volkInitialize();
	createInstance();
	setupDebugCallBack();
	volkLoadInstance(m_instance);
	createSurface();
	selectPhysicalDevice();
	createLogicalDevice();
	volkLoadDevice(m_device);
	createAllocator();
	createSwapChain();
	createCommandPools();
	createSyncObjects();
	createMainRenderPass();
	createSwapchainFramebuffers();
}

/**
 * Creates a Vulkan instance
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
	appinfo.pApplicationName = "Game";
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
 * Creates the surface needed as context for the Vulkan application.
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
 * Selects the most competent physical device to use vulkan with.
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


		if (m_physicalDevice == nullptr)
		{
			throw std::runtime_error("Failed to find GPU");

		}

	}






}
/**
 * Creates a logical device from the selected physical device.
 * 
 */
void PersistanceVkCore::createLogicalDevice()
{
	std::vector<VkDeviceQueueCreateInfo> queuecreateinfos;

	std::set<uint32_t>uniquequeuefamilies =
	{
		m_queueFamilyIndices.graphicsfamily,
		m_queueFamilyIndices.presentfamily,
		m_queueFamilyIndices.transferfamily

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



	if (vkCreateDevice(m_physicalDevice, &devicecreateinfo, nullptr, &m_device) != VK_SUCCESS)
	{
		throw std::runtime_error("Could not create logical device");

	}

	vkGetDeviceQueue(m_device, m_queueFamilyIndices.graphicsfamily, 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_device, m_queueFamilyIndices.presentfamily, 0, &m_presentQueue);
	vkGetDeviceQueue(m_device, m_queueFamilyIndices.transferfamily, 0, &m_transferQueue);

}

/**
 * Creates a Vulkan Memory Allocator.
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
 * Creates a swapchain.
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
 * Creates the Vulkan command pools.
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

}

/**
 * Creates syncronization semaphores and fences.
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
	s_renderFinished.resize(PersistanceLib::MAXFRAMESINFLIGHT);
	f_inFlightFence.resize(PersistanceLib::MAXFRAMESINFLIGHT);
	f_imagesInFlight.resize(3/*m_swapchainImages.size()*/, VK_NULL_HANDLE);


	for (int i = 0; i < PersistanceLib::MAXFRAMESINFLIGHT; i++)
	{
		if (vkCreateSemaphore(m_device, &semaphorecreateinfo, nullptr, &s_imageAvailable[i]) != VK_SUCCESS 
			|| vkCreateSemaphore(m_device, &semaphorecreateinfo, nullptr, &s_renderFinished[i]) != VK_SUCCESS 
			|| vkCreateFence(m_device, &fencecreateinfo, nullptr, &f_inFlightFence[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Semaphores or fences could not be initialized!");
		}

	}
}

/**
 * Creates the main renderpass for the engine.
 * 
 */
void PersistanceVkCore::createMainRenderPass()
{

	RenderPassAttachment colorattachment = RenderPassFunc::createRenderPassAttachment(0, 
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 
		m_swapchainImageFormat, 
		VK_SAMPLE_COUNT_1_BIT, 
		VK_ATTACHMENT_LOAD_OP_CLEAR, 
		VK_ATTACHMENT_STORE_OP_STORE, 
		VK_IMAGE_LAYOUT_UNDEFINED, 
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	);

	AttachmentReferenceList refList;
	refList.add(&colorattachment, 1);
	AttachmentDescriptionList desList;
	desList.add(&colorattachment, 1);

	VkSubpassDescription description = RenderPassFunc::createSubpassDescription(&refList, nullptr, nullptr, nullptr, 0);
	VkSubpassDependency dependency = RenderPassFunc::createSubpassDependency(VK_SUBPASS_EXTERNAL, 0, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0);
	

	m_mainRenderPass = RenderPassFunc::createRenderPass(&description, 1, &dependency, 1, desList);

}

/**
 * @brief creates the swapchain image views
 * 
 */
void PersistanceVkCore::createSwapchainImageViews()
{
	uint32_t size = m_swapchainFramebuffers.images.size();

	m_swapchainFramebuffers.imageviews.resize(size);

	for (int i = 0; i < size; i++) 
	{
		PersistanceUtils::createImageView(m_swapchainFramebuffers.images[i], m_swapchainImageFormat);

	}
	
}

void PersistanceVkCore::createSwapchainFramebuffers()
{
	uint32_t imageViewCount = m_swapchainFramebuffers.imageviews.size();

	for (int i = 0; i < imageViewCount; i++)
	{
		VkImageView attachments[] = {
			m_swapchainFramebuffers.imageviews[i]
		};

		VkFramebufferCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.height = m_swapchainExtent.height;
		info.width = m_swapchainExtent.width;
		info.layers = 1;
		info.renderPass = m_mainRenderPass.renderpass;
		info.attachmentCount = 1;
		info.pAttachments = attachments;
		info.flags = 0;

		if (vkCreateFramebuffer(m_device, &info, nullptr, &m_swapchainFramebuffers.framebuffers[i]) != VK_SUCCESS)
		{
			BREAK(1);
		}


	}

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

	VkDebugUtilsMessengerCreateInfoEXT createinfo;
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

