/*****************************************************************//**
 * @file   PersistanceVkCore.h
 * @brief  This is the core file for the PersistanceVulkan Framwework
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-29-2026
 *********************************************************************/
#pragma once
#include "../PersistanceLib.h"


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


#include "../Includes/glmIncludes.h"
#include "../Includes/stdLibIncludes.h"

#include "../Vendor/stb_image.h"
#include "../Structures/Framebuffer.h"
#include "../Structures/RenderPass.h"
#include "../Structures/Buffer.h"
#include "../Structures/Texture.h"


#include "../Debug/DebugUtilsMessengerEXT.h"


#define BREAK __debugbreak();

const uint32_t screenwidth = 800;
const uint32_t screenheight = 600;

/**
 * @brief A string containing the validation layers.
 */
const std::vector<const char*> m_validationlayers = {
	"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
const bool enablevalidationlayers = false;
#else
const bool enablevalidationlayers = true;
#endif

/**
 * @brief This struct provides the surface characteristics for the swapchain.
 */
struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR surfacecapabilities;
	std::vector<VkSurfaceFormatKHR> surfaceformat;
	std::vector<VkPresentModeKHR> presentmode;

};

/**
 * @brief Provides the queue family indices for a device.
 */
struct QueueFamilyIndices
{
	uint32_t graphicsfamily = -1;
	uint32_t presentfamily = -1;
	uint32_t computefamily = -1;
	uint32_t transferfamily = -1;


};


/**
 * @brief Manages core resources required to set up and implement Vulkan like a Vulkan Device, 
 * memory management, window creation through GLFW, Swapchain images, framebuffers, and renderpass, 
 * as well as validation layers.
 */
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
	Texture m_swapchainDepthBuffer;
	bool m_depthTesting = false;
	VkExtent2D m_swapchainExtent;
	VkRenderPass m_mainRenderPass;
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

	//Create
	void createInstance();
	void createSurface();
	void selectPhysicalDevice();
	void createLogicalDevice();
	void createAllocator();
	void createSwapChain();
	void createCommandPools();
	void createSyncObjects();


	void createSwapchainRenderPass();
	void createSwapchainImageViews();
	void createSwapchainFramebuffers(VkImageView* depthBufferImageView);
	void createSwapchainDepthBuffer();
	void recreateSwapchain();
	
	
	//Destroy and clean up
	void cleanUpSwapchain();
	void cleanUpMainRenderPass();
	void cleanUpSyncObjects();
	void cleanUpCommandPools();
	void cleanUpAllocator();



public:
	void createMainRenderSetup(bool doDepthTesting);
	void beginMainRenderPass(VkCommandBuffer& commandBuffer);
	void startDrawing();
	void endDrawingandPresent(VkCommandBuffer* commandBuffers, const uint32_t commandBufferCount);
	void bindGraphicsPipeline(VkCommandBuffer& commandBuffer, const VkPipeline& graphicsPipeline);
	void drawIndexed(VkCommandBuffer& commandBuffer, const Buffer* vertexBuffers, const uint32_t vertexBufferCount, const VkDeviceSize* offsets, Buffer& indexBuffer, VkPipeline& graphicsPipeline, VkPipelineLayout& graphicsPipelineLayout, const VkDescriptorSet* descriptorSets, uint32_t descriptorSetCount);
	void finalize();
private:

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
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		"VK_KHR_index_type_uint8"
	};

};

inline PersistanceVkCore core;


