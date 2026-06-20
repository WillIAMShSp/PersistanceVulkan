#pragma once

#include "PersistanceLib.h"


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
#include <array>
#include <unordered_map>
#include <optional>
#include <set>
#include <string>
#include <algorithm>
#include <fstream>
#include <chrono>

#include "../Settings/PipelineSettings.h"

#include "Debug/DebugUtilsMessengerEXT.h"


#include "Objects/Shader.h"
#include "Structures/Texture.h"
#include "Structures/UniformBuffer.h"
#include "Structures/DescriptorPool.h"
#include "Structures/DescriptorBufferInfo.h"
#include "Structures/DescriptorImageInfo.h"
#include "Structures/DescriptorSet.h"
#include "Structures/WriteDesciptorSet.h"
#include "Structures/DescriptorSetLayout.h"
#include "Structures/RenderPass.h"
#include "Structures/Buffer.h"
#include "Structures/GraphicsPipeline.h"
#include "Structures/Framebuffer.h"
#include "Structures/Drawable.h"






#ifndef NDEBUG

#ifdef _WIN32
#define ASSERT(x) if(!x) __debugbreak();
#endif
#endif

#define BREAK __debugbreak();



struct ModelViewProjectionBuffer
{
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 projection;

};










class PersistanceVk
{
public:
	void Init()
	{
		InitWindow();
		InitVulkan();
		
		

	}
	void End() 
	{
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
		volkInitialize();
		CreateInstance();
		volkLoadInstance(m_instance);
		SetUpDebugCallBack();
		CreateSurface();
		SelectPhysicalDevice();
		CreateLogicalDevice();
		volkLoadDevice(m_device);
		CreateAllocator();
		CreateSwapChain();
		CreateCommandPools();
		CreateSyncObjects();
	}

	
	void CleanUp()
	{


		CleanUpSwapchain();


		CleanTextures();
		
		CleanDescriptorPools();
		CleanDescriptorSetLayout();

		CleanUniformBuffers();

		CleanVertexBuffers();
		CleanIndexBuffers();

		CleanFramebuffers();

		for (int i = 0; i < PersistanceLib::MAXFRAMESINFLIGHT; i++)
		{
			vkDestroySemaphore(m_device, s_imageAvailable[i], nullptr);
			vkDestroySemaphore(m_device, s_renderFinished[i], nullptr);
			vkDestroyFence(m_device, f_inFlightFence[i], nullptr);

		}

		CleanRenderPass();

		vkDestroyCommandPool(m_device, m_graphicsCommandPool, nullptr);
		vkDestroyCommandPool(m_device, m_transferCommandPool, nullptr);

		CleanGraphicsPipelines();

		CleanAllocator();

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


public:

	static void ResizeWindowCallback(GLFWwindow* window, int width, int height)
	{
		auto app = reinterpret_cast<PersistanceVk*>(glfwGetWindowUserPointer(window));
		app->m_windowResized = true;


	}

	



private:

	void SetUpDebugCallBack();

	void CreateLogicalDevice();

	void CreateSurface();

	void SelectPhysicalDevice();

	void CreateInstance();

	void CreateSwapChain();

	void CreateCommandPools();

	void CreateSyncObjects();

	void CleanUpSwapchain();

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

	static std::vector<char> ReadFile(const char* filepath);

	VkShaderModule CreateShaderModule(const std::vector<char>& shaderfile);

	VkCommandBuffer BeginSingleTimeCommands(VkCommandPool& commandpool, const VkCommandBufferLevel& level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	void EndSingleTimeCommands(VkCommandBuffer& commandbuffer, const VkCommandPool& commandpool, const VkQueue& submitqueue);

	void RecreateSwapchain(uint32_t renderpasshandle);

	void CreateBuffer(const VkDeviceSize& size,
		VkBufferUsageFlags usageflags,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VmaAllocation& allocation,
		VkSharingMode sharingmode = VK_SHARING_MODE_EXCLUSIVE);

	void CreateImage(const uint32_t& width,
		const uint32_t height,
		VkFormat format,
		VkImageTiling tiling,
		const VkImageUsageFlags& usage,
		const VkMemoryPropertyFlags& properties,
		VkImage& image,
		VmaAllocation& allocation,
		VkSharingMode sharingmode = VK_SHARING_MODE_EXCLUSIVE, 
		VkImageLayout initiallayout = VK_IMAGE_LAYOUT_UNDEFINED);

	void CopyBuffer(VkBuffer& srcbuffer, VkBuffer& dstbuffer, VkDeviceSize size, VkCommandPool& commandpool, VkQueue& submitqueue);

	uint32_t FindMemoryType(uint32_t typefilter, VkMemoryPropertyFlags flags);

	void TransitionImageLayout(VkImage& image, const VkFormat& format, VkImageLayout oldlayout, VkImageLayout newlayout, VkCommandPool& commandpool, VkQueue submitqueue);

	void CopyBuffertoImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height, VkCommandPool& commandpool, VkQueue& submitqueue);

	VkImageView CreateImageView(VkImage& image, VkFormat format, VkImageAspectFlags imageaspect = VK_IMAGE_ASPECT_COLOR_BIT);

private: //member variables

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
	//std::vector<VkImage> m_swapchainImages;
	VkFormat m_swapchainImageFormat;
	VkExtent2D m_swapchainExtent;
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

	uint32_t m_renderPassCount = 0;
	std::vector<RenderPass> mh_renderPasses;
	std::vector<DescriptorSetLayout> mh_descriptorSetLayouts;
	uint32_t m_DSLHandleCount = 0;
	uint32_t m_pipelineHandleCount = 0;
	std::vector<GraphicsPipeline> mh_graphicsPipelines;
	uint32_t m_framebufferHandleCount = 0;
	std::vector<Framebuffer> mh_framebuffers;
	Framebuffer m_swapchainFramebuffers;
	std::vector<Texture> mh_textures;
	std::vector <VkSampler> mh_textureSamplers;
	uint32_t m_textureHandleCount = 0;
	uint32_t m_textureSamplerHandleCount = 0;
	std::vector<Buffer> mh_vertexBuffers;
	uint32_t m_vertexBufferHandleCount = 0;
	std::vector<Buffer> mh_indexBuffers;
	uint32_t m_indexBufferHandleCount = 0;
	std::vector<UniformBuffer> mh_uniformBuffers;
	uint32_t m_uniformBufferHandleCount = 0;
	uint32_t m_descriptorPoolHandleCount = 0;
	std::vector<DescriptorPool> mh_descriptorPools;
	uint32_t m_descriptorSetHandleCount = 0;
	std::vector<DescriptorSet>mh_descriptorSets;
	uint32_t m_commandBufferHandleCount = 0;
	std::vector<std::vector<VkCommandBuffer>> mh_commandBuffers;
	VmaAllocatorCreateInfo m_vmaAllocCreateInfo{};
	VmaAllocator m_vmaAllocator;
	VmaVulkanFunctions m_vmaFunctions;
	
	const std::vector<const char*> m_deviceextensions
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};


public:

	//Getters and Setters
	VkExtent2D& GetSwapchainExtent();
	VkFormat& GetSwapchainImageFormat();
	uint32_t GetCurrentFrame();
	VkCommandPool& GetGraphicsCommandPool();
	VkCommandPool& GetTransferCommandPool();

	void TransitionImageLayout(BufferHandle imageHandle, uint32_t imageindex, VkImageLayout oldLayout, VkImageLayout newLayout);

	//RenderPass modulation	

	RenderPassHandle CreateRenderPassHandle();
	uint32_t CreateRenderPassColorAttachment(RenderPassHandle handle,
		VkFormat format,
		VkSampleCountFlagBits imagesamples,
		VkAttachmentLoadOp loadop,
		VkAttachmentStoreOp storeop,
		VkImageLayout initialimagelayout,
		VkImageLayout finalimagelayout);
	uint32_t CreateRenderPassDepthStencilAttachment(RenderPassHandle handle,
		VkFormat format,
		VkSampleCountFlagBits imagesamples,
		VkAttachmentLoadOp loadop,
		VkAttachmentStoreOp storeop,
		VkAttachmentLoadOp depthstencilloadop,
		VkAttachmentStoreOp depthstencilstoreop,
		VkImageLayout initialimagelayout,
		VkImageLayout finalimagelayout);
	uint32_t CreateRenderPassInputAttachment(RenderPassHandle handle,
		VkFormat format,
		VkSampleCountFlagBits imagesamples,
		VkAttachmentLoadOp loadop,
		VkAttachmentStoreOp storeop,
		VkAttachmentLoadOp depthstencilloadop,
		VkAttachmentStoreOp depthstencilstoreop,
		VkImageLayout initialimagelayout,
		VkImageLayout finalimagelayout);
	uint32_t CreateSubpassDescription(RenderPassHandle handle,
		const uint32_t* colorattachmentindices = nullptr,
		size_t colorattachmentcount = 0,
		const uint32_t depthandstencilattachmentindex = UINT32_MAX,
		const uint32_t* inputattachmentindices = nullptr,
		const uint32_t inputattachmentcount = 0,
		const uint32_t* preserveattachmentindices = nullptr,
		const uint32_t preservedattachmentcount = 0);
	uint32_t CreateSubpassDependency(RenderPassHandle handle,
		uint32_t srcsubpass,
		uint32_t dstsubpass,
		VkPipelineStageFlags srcstagemask,
		VkPipelineStageFlags dststagemask,
		VkAccessFlags srcaccessmask,
		VkAccessFlags dstaccessmask);
	void CreateRenderPass(RenderPassHandle handle,
		const uint32_t* attachmentindicies,
		uint32_t attachmentcount,
		const uint32_t* subpassdescriptionindicies,
		uint32_t subpassdescriptioncount,
		const uint32_t* subpassdependencyindices,
		uint32_t subpassdependencycount);

	void CleanRenderPass();

	//Descriptor set layout modulation

	DescriptorSetLayoutHandle CreateDescriptorSetLayoutHandle(); //this fuction creates a handle for a descriptorsetlayout.
	void AddDescriptorSetLayoutBinding(DescriptorSetHandle handle, VkDescriptorSetLayoutBinding& binding); //The handle is used here to add bindings to the layout;
	void AddDescriptorSetLayoutBinding(DescriptorSetHandle handle, uint32_t bindingidx, VkDescriptorType descriptortype, VkShaderStageFlagBits shaderstage);
	void CreateDescriptorSetLayout(DescriptorSetHandle handle);
	void CleanDescriptorSetLayout();

	//Graphics pipeline modulation

	GraphicsPipelineHandle CreateGraphicsPipelineHandle(); //Creates a handle for a graphics pipeline;
	void AddVertexStage(GraphicsPipelineHandle handle, const char* shaderpath); //adds a vertex stage to the created pipeline shader.
	void AddFragmentStage(GraphicsPipelineHandle handle, const char* shaderpath); // adds a fragment stage to the created pipeline shader.

	void CreateGraphicsPipelineLayout(GraphicsPipelineHandle graphicspipelinehandle, uint32_t descriptorsetbinding);
	void CreateGraphicsPipeline(GraphicsPipelineHandle handle, PipelineSettings& settings, const uint32_t renderpasshandle);
	void DestroyShaders(GraphicsPipelineHandle handle);
	void CleanGraphicsPipelines();

	//Framebuffer modulation

	FramebufferHandle CreateFrameBuffersHandle();

	
	void CreateFramebufferImage(FramebufferHandle handle, int width, int height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageflags, VkMemoryPropertyFlags memoryproperties);
	void CreateFramebufferImageViews(FramebufferHandle handle, VkFormat format);
	void CreateFramebuffers(FramebufferHandle handle, const uint32_t renderpasshandle, uint32_t width, uint32_t height, uint32_t layers = 1);
	
	
	void CreateSwapchainFramebufferImageViews();
	void CreateSwapchainFramebuffers(const uint32_t renderpasshandle);


	void CleanFramebuffers();

	//Texture modulation

	uint32_t CreateTextureHandle();
	void CreateTextureImage(TextureHandle handle, int width, int height);
	void CreateTextureImage(TextureHandle handle, const char* imagesrc);
	void CreateTextureImageView(TextureHandle handle);
	void AddImageToTexture(TextureHandle handle, const char* imagesrc);

	TextureSamplerHandle CreateTextureSamplerHandle();
	void CreateTextureSampler(TextureSamplerHandle handle,
		VkFilter magfilter = VK_FILTER_LINEAR,
		VkFilter minfilter = VK_FILTER_LINEAR,
		VkSamplerAddressMode addressmodeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		VkSamplerAddressMode addressmodeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		VkSamplerAddressMode addressmodeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		VkBorderColor bordercolor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
		VkSamplerMipmapMode mipmapmode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		float miplodbias = 0.0f,
		float minlod = 0.0f,
		float maxlod = 0.0f,
		bool anisotropy = false);

	void CleanTextures();


	//Vertex buffer modulation

	BufferHandle CreateVertexBufferHandle();
	void CreateVertexBuffer(BufferHandle handle, const void* buffer, size_t elementsize, uint32_t elementcount);
	void CleanVertexBuffers();

	//Index buffer modulation

	BufferHandle CreateIndexBufferHandle();
	void CreateIndexBuffer(BufferHandle handle, void* buffer, uint32_t indexcount);

	void CleanIndexBuffers();

	//Uniform buffer modulation

	uint32_t CreateUniformBufferHandle();
	void CreateUniformBuffer(uint32_t handle, size_t buffersize);
	void UpdateUniformBuffer(uint32_t handle, const void* buffer, const uint32_t currentframe);

	void CleanUniformBuffers();

	//Descriptor pool modulation

	DescriptorPoolHandle CreateDescriptorPoolHandle();
	void AddDescriptorPoolSize(DescriptorPoolHandle handle, VkDescriptorType type);
	void CreateDescriptorPool(DescriptorPoolHandle handle);

	void CleanDescriptorPools();

	//Descriptor set modulation

	DescriptorSetHandle CreateDescriptorSetHandle();
	void CreateDescriptorSets(DescriptorSetHandle handle, uint32_t layouthandle, uint32_t poolhandle);
	WriteDescriptorSet* CreateWriteDescriptorSet(DescriptorSetHandle handle, uint32_t descriptorcount, uint32_t bindingidx, VkDescriptorType descriptortype, uint32_t* writedescriptorindex);
	void AddDescriptorBufferInfoToWriteDescriptorSet(DescriptorSetHandle handle, uint32_t writedescriptorindex, uint32_t uniformbufferhandle, size_t offset, size_t range);
	void AddDescriptorImageInfoToWriteDescriptorSet(DescriptorSetHandle handle, uint32_t writedescriptorindex, VkImageLayout imagelayout, TextureHandle texturehandle, TextureSamplerHandle texturesamplerhandle);

	/// Command Buffer Modulation

	BufferHandle CreateCommandBufferHandle();
	void CreateCommandBuffer(BufferHandle handle, VkCommandPool& commandpool, VkCommandBufferLevel level);


	
	
	void StartDrawing(BufferHandle commandbufferhandle, RenderPassHandle renderpasshandle);
	void BeginCommandBuffer(BufferHandle commandbufferhandle, VkCommandBufferUsageFlags flags);
	void BeginRenderPass(BufferHandle commandbufferhandle, RenderPassHandle renderpasshandle, bool usingswapchainframebuffer, FramebufferHandle framebufferhandle, VkClearValue clearcolor, VkOffset2D offset, VkExtent2D extent);
	void BindGraphicsPipeline(BufferHandle commandbufferhandle, VkPipelineBindPoint bindingpoint, GraphicsPipelineHandle handle);
	void SetViewport(BufferHandle commandbufferhandle, float xpos, float ypos, float mindepth, float maxdepth, VkExtent2D extent);
	void SetScissors(BufferHandle commandbufferhandle, VkOffset2D offset, VkExtent2D extent);
	void Draw(BufferHandle commandbufferhandle, Drawable drawsettings);
	void DrawIndexed(BufferHandle commandbufferhandle, Drawable drawsettings);
	void EndRenderPass(BufferHandle commandbufferhandle);
	void EndCommandBuffer(BufferHandle commandbufferhandle);
	void EndAndPresentDrawing(BufferHandle commandbufferhandle, RenderPassHandle renderpassHandle);

	//Vulkan memory allocator

	void CreateAllocator();
	void CleanAllocator();

	bool IsRunning() 
	{
		return !glfwWindowShouldClose(m_window);
	}
	void PollEvents() 
	{
		glfwPollEvents();
	}
	void WaitForDeviceIdle() 
	{
		vkDeviceWaitIdle(m_device);
	} 


};