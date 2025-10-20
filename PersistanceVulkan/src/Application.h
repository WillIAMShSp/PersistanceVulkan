#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "stb_image.h"


#include <iostream>
#include <vector>
#include <map>
#include <optional>
#include <set>
#include <algorithm>
#include <fstream>
#include <chrono>

#include "DebugUtilsMessengerEXT.h"




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

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsfamily;
	std::optional<uint32_t> presentfamily;
	std::optional<uint32_t> computefamily;
	std::optional<uint32_t> transferfamily;
	

};
struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR surfacecapabilities;
	std::vector<VkSurfaceFormatKHR> surfaceformat;
	std::vector<VkPresentModeKHR> presentmode;

};

struct ModelViewProjectionBuffer
{
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 projection;

};



class Application
{
public:
	void run()
	{
		InitWindow();
		InitVulkan();
		MainLoop();
		CleanUp();

	}

private:

	void InitWindow()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);


		m_window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

		std::cout << "Initialized Window \n";

		glfwSetWindowUserPointer(m_window, this);

		glfwSetWindowSizeCallback(m_window, ResizeWindowCallback);
		



	}

	void InitVulkan()
	{
		CreateInstance();
		SetUpDebugCallBack();
		CreateSurface();
		SelectPhysicalDevice();
		CreateLogicalDevice();
		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateDescriptorSetLayout();
		CreateGraphicsPipeline();
		CreateFramebuffers();
		CreateCommandPools();
		CreateTextureImage();
		CreateVertexBuffers();
		CreateIndexBuffers();
		CreateUniformBuffer();
		CreateDescriptorPool();
		CreateDescriptorSets();
		CreateCommandBuffer();
		CreateSyncObjects();

	}



	void MainLoop()
	{
		while (!glfwWindowShouldClose(m_window)) 
		{
			glfwPollEvents();
			
			DrawFrame();
		
		}

		vkDeviceWaitIdle(m_device);

	}

	void CleanUp()
	{


		CleanUpSwapchain();

		vkDestroyDescriptorPool(m_device, m_descriptorpool, nullptr);

		vkDestroyDescriptorSetLayout(m_device, m_descriptorsetlayout, nullptr);

		for (int i = 0; i < MAXFRAMESINFLIGHT; i++)
		{
			vkDestroyBuffer(m_device, m_uniformbuffers[i], nullptr);
			vkFreeMemory(m_device, m_uniformbuffermem[i], nullptr);

		}

		vkDestroyBuffer(m_device, m_vertexbuffer, nullptr);
		vkFreeMemory(m_device, m_vertexbuffermemory, nullptr);

		vkDestroyBuffer(m_device, m_indexbuffer, nullptr);
		vkFreeMemory(m_device, m_indexbuffermemory, nullptr);


		for (int i = 0; i < MAXFRAMESINFLIGHT; i++)
		{
			vkDestroySemaphore(m_device, s_imageavailable[i], nullptr);
			vkDestroySemaphore(m_device, s_renderfinished[i], nullptr);
			vkDestroyFence(m_device, f_inflightfence[i], nullptr);

		}

		
		
		vkDestroyCommandPool(m_device, m_graphicscommandpool, nullptr);
		vkDestroyCommandPool(m_device, m_transfercommandpool, nullptr);

		vkDestroyPipeline(m_device, m_pipeline, nullptr);
		vkDestroyPipelineLayout(m_device, m_pipelinelayout, nullptr);
		vkDestroyRenderPass(m_device, m_renderpass, nullptr);

		

		

		vkDestroyDevice(m_device, nullptr);

		if (enablevalidationlayers) 
		{
		
			DebugUtilsMessengerEXT::Destroy(m_instance, debugmessenger, nullptr);
		}

		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

		vkDestroyInstance(m_instance, nullptr);

		glfwDestroyWindow(m_window);

		
		glfwTerminate();

	}

public:

	static void ResizeWindowCallback(GLFWwindow* window, int width, int height) 
	{
		auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
		app->m_windowresized = true;

		
	}

	struct Vertex
	{
		glm::vec2 position;
		glm::vec3 color;

		static VkVertexInputBindingDescription GetBindingDescription()
		{
			VkVertexInputBindingDescription bindingdescription{};
			bindingdescription.binding = 0;
			bindingdescription.stride = sizeof(Vertex);
			bindingdescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			


			return bindingdescription;

		}

		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescription()
		{
			std::vector<VkVertexInputAttributeDescription> attributedescription{};
			attributedescription.resize(2);
			
			attributedescription[0].binding = 0;
			attributedescription[0].location = 0;
			attributedescription[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributedescription[0].offset = offsetof(Vertex, position);

			attributedescription[1].binding = 0;
			attributedescription[1].location = 1;
			attributedescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributedescription[1].offset = offsetof(Vertex, color);

			return attributedescription;

		}


	};



private:

	void SetUpDebugCallBack();
	
	void CreateLogicalDevice();

	void CreateSurface();
	
	void SelectPhysicalDevice();

	void CreateInstance();

	void CreateSwapChain();

	void CreateImageViews();

	void CreateRenderPass();

	void CreateDescriptorSetLayout();

	void CreateDescriptorSets();

	void CreateGraphicsPipeline();

	void CreateFramebuffers();

	void CreateCommandPools();

	void CreateUniformBuffer();
	
	void CreateDescriptorPool();

	void CreateVertexBuffers();

	void CreateIndexBuffers();

	void CreateCommandBuffer();

	void CreateSyncObjects();

	void CleanUpSwapchain();

	void CreateTextureImage();

	bool CheckValidationLayers();

	std::vector<const char*> GetRequiredInstanceExtensions();

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallBack(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	void SetDebugCreateInfoStructVariables(VkDebugUtilsMessengerCreateInfoEXT& createinfo);

	
	
	bool RateDevice(VkPhysicalDevice& physicaldevice, uint32_t& scorehandle, bool& presentfamily, VkPhysicalDeviceProperties* propertieshandle = nullptr);

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice& physicaldevice);

	bool DeviceExtensionSupport(VkPhysicalDevice& physicaldevice);

	
	
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice& physicaldevice);

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableformats);

	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablepresentmodes);

	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfacecapabilities);

	static std::vector<char> ReadFile(const std::string& filepath);
	
	VkShaderModule CreateShaderModule(const std::vector<char>& shaderfile);

	VkCommandBuffer BeginSingleTimeCommands(VkCommandPool& commandpool, const VkCommandBufferLevel& level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	void EndSingleTimeCommands(VkCommandBuffer& commandbuffer, const VkCommandPool& commandpool, const VkQueue& submitqueue);

	void RecordCommandBuffer(VkCommandBuffer& commandbuffer, const uint32_t& swapchainimageindex);

	void DrawFrame();

	void RecreateSwapchain();

	void CreateBuffer(const VkDeviceSize& size, VkBufferUsageFlags usageflags, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& buffermemory, VkSharingMode sharingmode = VK_SHARING_MODE_EXCLUSIVE);

	void CreateImage(const uint32_t& width, const uint32_t height, VkFormat format, VkImageTiling tiling, const VkImageUsageFlags&  usage, const VkMemoryPropertyFlags& properties, VkImage& image, VkDeviceMemory& imagememory, VkSharingMode sharingmode = VK_SHARING_MODE_EXCLUSIVE);
	
	void CopyBuffer(VkBuffer& srcbuffer, VkBuffer& dstbuffer, VkDeviceSize size, VkCommandPool& commandpool, VkQueue& submitqueue);

	uint32_t FindMemoryType(uint32_t typefilter, VkMemoryPropertyFlags flags);

	void UpdateUniformBuffer(const uint32_t& currentframe);



private:

	GLFWwindow* m_window = nullptr;
	VkInstance m_instance = nullptr;
	VkDebugUtilsMessengerEXT debugmessenger;
	VkPhysicalDevice m_physicaldevice = VK_NULL_HANDLE;
	QueueFamilyIndices m_queuefamilyindices;


	VkDevice m_device;
	VkQueue m_graphicsqueue;
	VkQueue m_presentqueue;
	VkQueue m_transferqueue;
	VkSurfaceKHR m_surface;
	VkSwapchainKHR m_swapchain;
	std::vector<VkImage> m_swapchainimages;
	VkFormat m_swapchainimageformat;
	VkExtent2D m_swapchainextent;
	std::vector<VkImageView> m_swapchainimageviews;
	VkRenderPass m_renderpass;
	VkDescriptorSetLayout m_descriptorsetlayout;
	VkDescriptorPool m_descriptorpool;
	std::vector<VkDescriptorSet> m_descriptorsets;
	VkPipelineLayout m_pipelinelayout;
	VkPipeline m_pipeline;
	std::vector<VkFramebuffer> m_swapchainframebuffers;
	VkCommandPool m_graphicscommandpool;
	VkCommandPool m_transfercommandpool;
	std::vector<VkCommandBuffer> m_commandbuffers;

	std::vector<VkSemaphore> s_imageavailable;
	std::vector<VkSemaphore> s_renderfinished;
	std::vector<VkFence> f_inflightfence;
	bool m_windowresized = false;

	VkBuffer m_vertexbuffer;
	VkDeviceMemory m_vertexbuffermemory;
	VkBuffer m_indexbuffer;
	VkDeviceMemory m_indexbuffermemory;
	std::vector<VkBuffer> m_uniformbuffers;
	std::vector<VkDeviceMemory> m_uniformbuffermem;
	std::vector<void*> m_uniformbuffersmapped;

	//textures
	VkImage m_textureimage;
	VkDeviceMemory m_textureimagemem;



	const std::vector<const char*> m_deviceextensions
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

private:
	//bool IsDeviceSuitable(VkPhysicalDevice& physicaldevice);
	
	const uint32_t MAXFRAMESINFLIGHT = 2;
	uint32_t m_currentframe = 0;



	const std::vector<Vertex> vertices = {
	
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	
	};
	const std::vector<uint16_t> indices =
	{
		0, 1, 2, 2, 3, 0

	};
	

	


};