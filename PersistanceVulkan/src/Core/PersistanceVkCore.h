#pragma once
#include "./PersistanceLib.h"


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


#include "../Includes/glmIncludes.h"
#include "../Includes/stdLibIncludes.h"

#include "stb_image.h"
#include "../Structures/Framebuffer.h"
#include "../Structures/RenderPass.h"

#include "Debug/DebugUtilsMessengerEXT.h"


#define BREAK __debugbreak();

const uint32_t screenwidth = 800;
const uint32_t screenheight = 600;

const std::vector<const char*> m_validationlayers = {
	"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
const bool enablevalidationlayers = false;
#else
const bool enablevalidationlayers = true;
#endif

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR surfacecapabilities;
	std::vector<VkSurfaceFormatKHR> surfaceformat;
	std::vector<VkPresentModeKHR> presentmode;

};


struct QueueFamilyIndices
{
	uint32_t graphicsfamily = -1;
	uint32_t presentfamily = -1;
	uint32_t computefamily = -1;
	uint32_t transferfamily = -1;


};



class PersistanceVkCore
{
public:
	
	GLFWwindow* m_window = nullptr;
	VkInstance m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	QueueFamilyIndices m_queueFamilyIndices;
	VkDevice m_device;
	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;
	VkQueue m_transferQueue;
	VkSurfaceKHR m_surface;
	VkSwapchainKHR m_swapchain;
	VkFormat m_swapchainImageFormat;
	Framebuffer m_swapchainFramebuffers;
	VkExtent2D m_swapchainExtent;
	RenderPass m_mainRenderPass;
	VkCommandPool m_graphicsCommandPool;
	VkCommandPool m_transferCommandPool;

	std::vector<VkSemaphore> s_imageAvailable;
	std::vector<VkSemaphore> s_renderFinished;
	std::vector<VkFence> f_inFlightFence;
	std::vector<VkFence> f_imagesInFlight;
	bool m_windowResized = false;

	uint32_t m_currentFrame = 0;
	uint32_t m_imageIndex = 0;
	bool m_currentlyDrawing = false;

	VmaAllocator m_vmaAllocator;
	VmaVulkanFunctions m_vmaFunctions;
	VmaAllocatorCreateInfo m_vmaAllocCreateInfo{};
	

	void init() {
		initWindow();
		initVulkan();
	}
	bool isRunning()
	{
		return !glfwWindowShouldClose(m_window);
	}
	void pollEvents()
	{
		glfwPollEvents();
	}
	void waitForDeviceIdle()
	{
		vkDeviceWaitIdle(m_device);
	}

private:
	void initWindow();
	void initVulkan();

	void createInstance();
	void createSurface();
	void selectPhysicalDevice();
	void createLogicalDevice();
	void createAllocator();
	void createSwapChain();
	void createCommandPools();
	void createSyncObjects();


	void createMainRenderPass();
	void createSwapchainImageViews();
	void createSwapchainFramebuffers();
	void beginMainRenderPass(VkCommandBuffer& commandBuffer);
	


	bool checkValidationLayers();
	void setupDebugCallBack();

	std::vector<const char*> getRequiredExtensions();

	static void resizeWindowCallback(GLFWwindow* window, int width, int height)
	{
		auto app = reinterpret_cast<PersistanceVkCore*>(glfwGetWindowUserPointer(window));
		app->m_windowResized = true;


	}

	bool deviceExtensionSupport(VkPhysicalDevice& physicaldevice);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice& physicaldevice);
	bool rateDevice(VkPhysicalDevice& physicaldevice, uint32_t& scorehandle, bool& presentfamily, VkPhysicalDeviceProperties* propertieshandle = nullptr);

	SwapChainSupportDetails querySwapchainSupportDetails(VkPhysicalDevice& physicaldevice);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableformats);

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablepresentmodes);

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfacecapabilities);

	const std::vector<const char*> m_deviceextensions
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

};

inline PersistanceVkCore core;


