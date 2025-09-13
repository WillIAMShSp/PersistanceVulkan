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
					std::cout << "\n  Using: " << deviceproperties.deviceName << "\n \n";
					
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

	if (!requiredextensionsupport)
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

	

	for (unsigned int i = 0; i < familycount; i++)
	{
		if (familyproperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsfamily = i;


			

		}
		VkBool32 presentsupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicaldevice, i, m_surface, &presentsupport);
		if (presentsupport)
		{
			indices.presentfamily = i;

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




