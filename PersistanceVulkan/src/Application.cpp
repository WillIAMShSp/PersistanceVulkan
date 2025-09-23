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
		VkImageViewCreateInfo createinfo{};
		createinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createinfo.image = m_swapchainimages[i];
		
		createinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createinfo.format = m_swapchainimageformat;

		createinfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createinfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createinfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createinfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createinfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createinfo.subresourceRange.baseArrayLayer = 0;
		createinfo.subresourceRange.layerCount = 1;
		createinfo.subresourceRange.levelCount = 1;
		createinfo.subresourceRange.baseMipLevel = 0;

		if (vkCreateImageView(m_device, &createinfo, nullptr, &m_swapchainimageviews[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create Image View");

		}




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
	
	vkDestroyShaderModule(m_device, vertexmodule, nullptr);
	vkDestroyShaderModule(m_device, fragmentmodule, nullptr);


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
		
		else if (familyproperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
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
		m_queuefamilyindices.presentfamily.value()

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




