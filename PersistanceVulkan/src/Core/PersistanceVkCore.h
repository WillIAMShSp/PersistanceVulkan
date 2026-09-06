/*****************************************************************//**
 * @file   PersistanceVkCore.h
 * @brief  This is the core file for the PersistanceVulkan Framwework
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-29-2026
 *********************************************************************/
#pragma once
#include "../PersistanceLib.h"
#include "vulkan/vulkan_core.h"


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


#include "../Includes/glmIncludes.h"
#include "../Includes/stdLibIncludes.h"

#include "../Vendor/stb_image.h"
#include "../Structures/Framebuffer.h"
#include "../Structures/Buffer.h"
#include "../Structures/Texture.h"


#include "../VulkanDebug/DebugUtilsMessengerEXT.h"


#define BREAK __debugbreak();


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

	//Getters and Setters

	GLFWwindow* getWindow();
	void setWindow(GLFWwindow* window);

	const VkInstance& getInstance();

	const VkPhysicalDevice& getPhysicalDevice();

	QueueFamilyIndices* getQueueFamilyIndices();

	const VkDevice& getDevice();

	VkQueue& getGraphicsQueue();
	VkQueue& getPresentQueue();
	VkQueue& getTransferQueue();
	VkQueue& getComputeQueue();

	const VkSwapchainKHR& getSwapchain();
	const VkFormat getSwapchainFormat();
	Framebuffer* getSwapchainFramebuffers();
	VkRenderPass& getMainRenderPass();
	Texture* getSwapchainDepthBuffer();
	bool swapchainIsDepthTesting();
	VkExtent2D getSwapchainExtent();

	VkCommandPool& getGraphicsCommandPool();
	VkCommandPool& getTransferCommandPool();
	VkCommandPool& getComputeCommandPool();

	uint32_t getCurrentFrame();
	uint32_t getImageIndex();

	uint32_t getScreenWidth();
	uint32_t getScreenHeight();

	const VmaAllocator& getAllocator();

	void addExtension(const char* extensionName);

	bool windowResized();

	void init(uint32_t width = 800, uint32_t height = 600, const void* deviceFeatures = nullptr) {
		initWindow(width, height);
		initVulkan(deviceFeatures);
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

	static void resizeWindowCallback(GLFWwindow* window, int width, int height)
	{
		auto app = reinterpret_cast<PersistanceVkCore*>(glfwGetWindowUserPointer(window));
		app->m_windowResized = true;
	}

private:
	void initWindow(const uint32_t screenWidth, const uint32_t screenHeight);
	void initVulkan(const void* deviceFeatures);

	//Create
	void createInstance();
	void createSurface();
	void selectPhysicalDevice();
	void createLogicalDevice(const void* deviceFeatures);
	void createAllocator();
	void createSwapChain();
	void createCommandPools();
	void createSyncObjects();


	void createSwapchainRenderPass();
	void createSwapchainImageViews();
	void createSwapchainFramebuffers(VkImageView* depthBufferImageView);
	void createSwapchainDepthBuffer();
	//void recreateSwapchain();
	
	
	//Destroy and clean up
	void cleanUpSwapchain();
	void cleanUpMainRenderPass();
	void cleanUpSyncObjects();
	void cleanUpCommandPools();
	void cleanUpAllocator();



public:
	void createMainRenderSetup(bool doDepthTesting);
	void beginMainRenderPass(VkCommandBuffer& commandBuffer, const uint32_t* imageIndex = nullptr);
	
	void startDrawing();
	void waitForCurrentFence();
	VkResult acquireNextSwapchainImage();
	void reRecordCommandBuffersCallBack(std::function<void()>* functions, const uint32_t count);


	void recreateSwapchain();
	

	void endDrawingandPresent(VkCommandBuffer* commandBuffers, const uint32_t commandBufferCount);
	void bindGraphicsPipeline(VkCommandBuffer& commandBuffer, const VkPipeline& graphicsPipeline);
	void drawIndexed(VkCommandBuffer& commandBuffer, const Buffer* vertexBuffers, const uint32_t vertexBufferCount, const VkDeviceSize* offsets, Buffer& indexBuffer, VkPipeline& graphicsPipeline, VkPipelineLayout& graphicsPipelineLayout, const VkDescriptorSet* descriptorSets, uint32_t descriptorSetCount);
	void drawIndexedIndirect(VkCommandBuffer &commandBuffer, Buffer& indirectBuffer, Buffer &indexBuffer);
	void finalize();
private:

	bool checkValidationLayers();
	void setupDebugCallBack();

	std::vector<const char*> getRequiredExtensions();

	
	bool deviceExtensionSupport(VkPhysicalDevice& physicaldevice);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice& physicaldevice);
	bool rateDevice(VkPhysicalDevice& physicaldevice, uint32_t& scorehandle, bool& presentfamily, VkPhysicalDeviceProperties* propertieshandle = nullptr);

	SwapChainSupportDetails querySwapchainSupportDetails(VkPhysicalDevice& physicaldevice);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableformats);

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablepresentmodes);

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfacecapabilities);

	std::vector<const char*> m_deviceextensions
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	uint32_t m_screenWidth = 800;
	uint32_t m_screenHeight = 600;
	GLFWwindow* m_window = nullptr;
	VkInstance m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	QueueFamilyIndices m_queueFamilyIndices;
	VkDevice m_device;
	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;
	VkQueue m_transferQueue;
	VkQueue m_computeQueue;
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
	VkCommandPool m_computeCommandPool;

	std::vector<VkSemaphore> s_imageAvailable;
	std::vector<VkSemaphore> s_renderFinished;
	std::vector<VkFence> f_inFlightFence;
	bool m_windowResized = false;

	uint32_t m_currentFrame = 0;
	uint32_t m_imageIndex = 0;
	bool m_currentlyDrawing = false;

	VmaAllocator m_vmaAllocator;
	VmaVulkanFunctions m_vmaFunctions;
	VmaAllocatorCreateInfo m_vmaAllocCreateInfo{};
	

};

inline PersistanceVkCore core;


