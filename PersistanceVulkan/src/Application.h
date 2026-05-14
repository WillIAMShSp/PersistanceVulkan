#pragma once

#include "PersistanceLib.h"

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

#include "DebugUtilsMessengerEXT.h"


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






#ifndef NDEBUG

#ifdef _WIN32
#define ASSERT(x) if(!x) __debugbreak();
#endif
#endif

typedef uint32_t FrameBufferHandle;
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
	uint32_t graphicsfamily = -1;
	uint32_t presentfamily= -1;
	uint32_t computefamily= -1;
	uint32_t transferfamily = -1;
	

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
#pragma region InitFunc
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
		CreateImageViews();
		CreateCommandPools();
#pragma endregion
		uint32_t renderpasshandle = CreateRenderPassHandle();
		uint32_t colorattachment = CreateRenderPassColorAttachment(
			renderpasshandle, 
			m_swapchainimageformat, 
			VK_SAMPLE_COUNT_1_BIT, 
			VK_ATTACHMENT_LOAD_OP_CLEAR, 
			VK_ATTACHMENT_STORE_OP_STORE, 
			VK_IMAGE_LAYOUT_UNDEFINED, 
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		uint32_t subpassdescription = CreateSubpassDescription(renderpasshandle, &colorattachment, 1);
		uint32_t subpassdependency = CreateSubpassDependency(
			renderpasshandle, 
			VK_SUBPASS_EXTERNAL, 
			0, 
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 
			0, 
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
		CreateRenderPass(renderpasshandle, &colorattachment, 1, &subpassdescription, 1, &subpassdependency, 1);
		CreateSwapchainFramebuffers(renderpasshandle);

#pragma region done

		//////// DescriptorSetLayouts
		
		int descriptorsetlayouthandle;
		descriptorsetlayouthandle = CreateDescriptorSetLayoutHandle();
		AddDescriptorSetLayoutBinding(descriptorsetlayouthandle, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		AddDescriptorSetLayoutBinding(descriptorsetlayouthandle, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

		
		CreateDescriptorSetLayout(descriptorsetlayouthandle);

		


		//The way this works is we create a layout handle and we create bindings associating it to that handle. we create a descriptorsetlayout associated with that handle. For now, it becomes the used descriptorsetlayout.


		


		//CreateDescriptorSetLayout();
		//CreateGraphicsPipeline();
///////////////////////////////////////////////////
		uint32_t pipeline = CreateGraphicsPipelineHandle();
		 
		AddVertexStage(pipeline, "res/Shaders/basicvert.spv"); /*Create shaders*/
		AddFragmentStage(pipeline, "res/Shaders/basicfrag.spv"); //
		CreateGraphicsPipelineLayout(pipeline, descriptorsetlayouthandle);
		PipelineSettings settings;
		VertexInputStateLayout vertexbufferlayout;
		vertexbufferlayout.push<glm::vec2>();/*Configure vertex array layout*/
		vertexbufferlayout.push<glm::vec3>();//
		vertexbufferlayout.push<glm::vec2>();//
		settings.CreateVertexInputState(vertexbufferlayout);
		settings.DefineInputAssemblyState();
		settings.CreateStaticViewPort();
		settings.ConfigureRasterizationStage();
		settings.ConfigureMultisample();
		settings.ConfigureColorBlend();
		settings.UseDynamicViewport();
		CreateGraphicsPipeline(pipeline, settings, renderpasshandle);
		m_pipeline = mh_graphicspipelines.at(pipeline).pipeline;
		m_pipelinelayout = mh_graphicspipelines.at(pipeline).layout;



//////////////////////////////////////////////////
#pragma endregion
		FrameBufferHandle frmbffrhndl = CreateFrameBuffersHandle();
		CreateFramebufferImage(frmbffrhndl);
		CreateFramebufferImageViews(frmbffrhndl);
		CreateFramebuffers(frmbffrhndl, renderpasshandle);

//////////////////////////////////////////////////

		CreateTextureSampler();
		uint32_t texturehandle = CreateTextureHandle();
		CreateTextureImage(texturehandle, "res/Textures/Placeholder.png");
		CreateTextureImageView(texturehandle);
		

//////////////////////////////////////////////////
		
		///////////////////////////
		uint32_t vertexbufferhndl = CreateVertexBufferHandle();
		CreateVertexBuffer(vertexbufferhndl, vertices.data(), sizeof(vertices[0]), vertices.size());

		/////TESTVB/////
		uint32_t testvbhandl = CreateVertexBufferHandle();
		CreateVertexBuffer(testvbhandl, TESTvertices.data(), sizeof(TESTvertices[0]), TESTvertices.size());
		////////////////


		uint32_t indexbufferhndl = CreateIndexBufferHandle();
		CreateIndexBuffer(indexbufferhndl, (void*)indices.data(), indices.size());

////////////////////////////////////

		
		//CreateUniformBuffer();

		////////////////////////

		uint32_t uniformbufferhandle = CreateUniformBufferHandle();
		CreateUniformBuffer(uniformbufferhandle, sizeof(ModelViewProjectionBuffer));
////////////////////////////////////
		
		//CreateDescriptorPool();

		////////////////////////

		uint32_t descriptorpoolhandle = CreateDescriptorPoolHandle();
		AddDescriptorPoolSize(descriptorpoolhandle, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		AddDescriptorPoolSize(descriptorpoolhandle, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		CreateDescriptorPool(descriptorpoolhandle);

		

////////////////////////////////////
		//CreateDescriptorSets();
		////////////////////////
		uint32_t descriptorsethandle = CreateDescriptorSetHandle();
		uint32_t uniformbufferwritedescriptor;
		uint32_t texturewritedescriptor;
		CreateWriteDescriptorSet(descriptorsethandle, 1, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &uniformbufferwritedescriptor);
		AddDescriptorBufferInfoToWriteDescriptorSet(descriptorsethandle, uniformbufferwritedescriptor, uniformbufferhandle, 0, sizeof(ModelViewProjectionBuffer));
		CreateWriteDescriptorSet(descriptorsethandle, 1, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &texturewritedescriptor);
		AddDescriptorImageInfoToWriteDescriptorSet(descriptorsethandle, texturewritedescriptor, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mh_textures.at(texturehandle).imageview, m_texsampler);
		CreateDescriptorSets(descriptorsethandle, descriptorsetlayouthandle, descriptorpoolhandle);

////////////////////////////////////
		//CreateCommandBuffer();
		//////////////////////
		uint32_t commandbufferhandle = CreateCommandBufferHandle();
		CreateCommandBuffer(commandbufferhandle, m_graphicscommandpool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);




		CreateSyncObjects();

	}



	void MainLoop()
	{
		while (!glfwWindowShouldClose(m_window)) 
		{
			glfwPollEvents();
			
			DrawFrame(0, 0, 0, 0, 0, 0);
		
		}

		vkDeviceWaitIdle(m_device);

	}

	void CleanUp()
	{


		CleanUpSwapchain();
		

		CleanTextures();
		vkDestroySampler(m_device, m_texsampler, nullptr);
		
		CleanDescriptorPools();
		CleanDescriptorSetLayout();

		vkDestroyDescriptorPool(m_device, m_descriptorpool, nullptr);
		CleanUniformBuffers();

		CleanVertexBuffers();
		CleanIndexBuffers();

		CleanFramebuffers();

		for (int i = 0; i < PersistanceLib::MAXFRAMESINFLIGHT; i++)
		{
			vkDestroySemaphore(m_device, s_imageavailable[i], nullptr);
			vkDestroySemaphore(m_device, s_renderfinished[i], nullptr);
			vkDestroyFence(m_device, f_inflightfence[i], nullptr);

		}

		CleanRenderPass();

		vkDestroyCommandPool(m_device, m_graphicscommandpool, nullptr);
		vkDestroyCommandPool(m_device, m_transfercommandpool, nullptr);

		CleanGraphicsPipelines();

		CleanAllocator();

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
		glm::vec2 uv;

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
			attributedescription.resize(3);
			
			attributedescription[0].binding = 0;
			attributedescription[0].location = 0;
			attributedescription[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributedescription[0].offset = offsetof(Vertex, position);

			attributedescription[1].binding = 0;
			attributedescription[1].location = 1;
			attributedescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributedescription[1].offset = offsetof(Vertex, color);

			attributedescription[2].binding = 0;
			attributedescription[2].location = 2;
			attributedescription[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributedescription[2].offset = offsetof(Vertex, uv);


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

	void CreateSwapchainFramebuffers(uint32_t renderpasshandle);

	void CreateDescriptorSetLayout();

	void CreateDescriptorSets();

	void CreateCommandPools();
	
	void CreateDescriptorPool();

	void CreateCommandBuffer();

	void CreateSyncObjects();

	void CleanUpSwapchain();

	void CreateTextureImageView();

	void CreateTextureSampler();

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

	void DrawFrame(const uint32_t commandbufferhandle, const uint32_t graphicspipelinehandle, const uint32_t vertexbufferhandle, const uint32_t indexbufferhandle, const uint32_t descriptorsethandle, const uint32_t renderpasshandle);

	void RecreateSwapchain(uint32_t renderpasshandle);

	void CreateBuffer(
		const VkDeviceSize& size,
		VkBufferUsageFlags usageflags,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VmaAllocation& allocation,
		VkSharingMode sharingmode = VK_SHARING_MODE_EXCLUSIVE
	);

	void CreateImage(const uint32_t& width, const uint32_t height, VkFormat format, VkImageTiling tiling, const VkImageUsageFlags&  usage, const VkMemoryPropertyFlags& properties, VkImage& image, VkDeviceMemory& imagememory, VkSharingMode sharingmode = VK_SHARING_MODE_EXCLUSIVE);
	
	void CopyBuffer(VkBuffer& srcbuffer, VkBuffer& dstbuffer, VkDeviceSize size, VkCommandPool& commandpool, VkQueue& submitqueue);

	uint32_t FindMemoryType(uint32_t typefilter, VkMemoryPropertyFlags flags);

	void UpdateUniformBuffer(const uint32_t& currentframe);

	void TransitionImageLayout(VkImage& image, const VkFormat& format, VkImageLayout oldlayout, VkImageLayout newlayout, VkCommandPool& commandpool, VkQueue submitqueue);

	void CopyBuffertoImage(VkBuffer& buffer, VkImage& image, uint32_t width ,uint32_t height, VkCommandPool& commandpool, VkQueue& submitqueue);

	VkImageView CreateImageView(VkImage& image, VkFormat format, VkImageAspectFlags imageaspect = VK_IMAGE_ASPECT_COLOR_BIT);

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
	std::vector<VkFence> f_imagesinflight;
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

	VkImageView m_teximageview;

	VkSampler m_texsampler;

	public:
	
	//RenderPass modulation	
		
	uint32_t m_renderpasscount = 0;
	
	std::vector<RenderPass> mh_renderpasses;

	uint32_t CreateRenderPassHandle(); 
	
	
	uint32_t CreateRenderPassColorAttachment(
		uint32_t handle,
		VkFormat format,
		VkSampleCountFlagBits imagesamples,
		VkAttachmentLoadOp loadop,
		VkAttachmentStoreOp storeop,
		VkImageLayout initialimagelayout,
		VkImageLayout finalimagelayout
		);
	uint32_t CreateRenderPassDepthStencilAttachment(
		uint32_t handle,
		VkFormat format,
		VkSampleCountFlagBits imagesamples,
		VkAttachmentLoadOp loadop,
		VkAttachmentStoreOp storeop,
		VkAttachmentLoadOp depthstencilloadop,
		VkAttachmentStoreOp depthstencilstoreop,
		VkImageLayout initialimagelayout,
		VkImageLayout finalimagelayout
	);

	uint32_t CreateRenderPassInputAttachment(
		uint32_t handle,
		VkFormat format,
		VkSampleCountFlagBits imagesamples,
		VkAttachmentLoadOp loadop,
		VkAttachmentStoreOp storeop,
		VkAttachmentLoadOp depthstencilloadop,
		VkAttachmentStoreOp depthstencilstoreop,
		VkImageLayout initialimagelayout,
		VkImageLayout finalimagelayout
	);

	uint32_t CreateRenderpassAttachment(
		uint32_t handle, 
		VkImageLayout attachmentlayout, 
		VkFormat format, 
		VkSampleCountFlagBits imagesamples, 
		VkAttachmentLoadOp loadop, 
		VkAttachmentStoreOp storeop, 
		VkImageLayout initialimagelayout, 
		VkImageLayout finalimagelayout
	);

	uint32_t CreateSubpassDescription(
		uint32_t handle,
		const uint32_t* colorattachmentindices = nullptr,
		size_t colorattachmentcount = 0,
		const uint32_t depthandstencilattachmentindex =  UINT32_MAX,
		const uint32_t* inputattachmentindices = nullptr,
		const uint32_t inputattachmentcount = 0,
		const uint32_t* preserveattachmentindices = nullptr,
		const uint32_t preservedattachmentcount = 0
	);

	uint32_t CreateSubpassDependency(
		uint32_t handle,
		uint32_t srcsubpass, 
		uint32_t dstsubpass, 
		VkPipelineStageFlags srcstagemask, 
		VkPipelineStageFlags dststagemask, 
		VkAccessFlags srcaccessmask, 
		VkAccessFlags dstaccessmask
	);

	void CreateRenderPass(
		uint32_t handle, 
		const uint32_t* attachmentindicies, 
		uint32_t attachmentcount, 
		const uint32_t* subpassdescriptionindicies, 
		uint32_t subpassdescriptioncount, 
		const uint32_t* subpassdependencyindices, 
		uint32_t subpassdependencycount
	);
	
	void CleanRenderPass();
		
	//Descriptor set layout modulation

	std::vector<DescriptorSetLayout> mh_descriptorsetlayouts;

	uint8_t m_dslhandlecount = 0;
	uint32_t CreateDescriptorSetLayoutHandle(); //this fuction creates a handle for a descriptorsetlayout.
	void AddDescriptorSetLayoutBinding(uint32_t handle, VkDescriptorSetLayoutBinding& binding); //The handle is used here to add bindings to the layout;
	void AddDescriptorSetLayoutBinding(uint32_t handle, uint32_t bindingidx, VkDescriptorType descriptortype, VkShaderStageFlagBits shaderstage);
	void CreateDescriptorSetLayout(uint32_t handle);

	void CleanDescriptorSetLayout();

	//Graphics pipeline modulation
	uint32_t m_pipelinecount = 0;
	
	std::vector<GraphicsPipeline> mh_graphicspipelines;

	uint32_t CreateGraphicsPipelineHandle(); //Creates a handle for a graphics pipeline;
	void AddVertexStage(uint32_t handle, const char* shaderpath); //adds a vertex stage to the created pipeline shader.
	void AddFragmentStage(uint32_t handle, const char* shaderpath); // adds a fragment stage to the created pipeline shader.

	void CreateGraphicsPipelineLayout(uint32_t graphicspipelinehandle, uint32_t descriptorsetbinding);
	void CreateGraphicsPipeline(uint32_t handle, PipelineSettings& settings, const uint32_t renderpasshandle);
	void DestroyShaders(uint32_t handle);
	void CleanGraphicsPipelines();
	 
	//Framebuffer modulation
	

	uint32_t m_framebuffercount = 0;

	std::vector<Framebuffer> mh_framebuffers;


	FrameBufferHandle CreateFrameBuffersHandle();

	void CreateFramebufferImage(FrameBufferHandle handle);
	void CreateFramebufferImage(FrameBufferHandle handle, int width, int height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageflags, VkMemoryPropertyFlags memoryproperties);
	void CreateFramebufferImageViews(FrameBufferHandle handle);
	void CreateFramebuffers(FrameBufferHandle handle, const uint32_t renderpasshandle);
	
	void CleanFramebuffers();

	//Texture modulation


	std::vector<Texture> mh_textures;
	std::unordered_map<uint32_t, VkSampler> mh_texturesamplers;

	uint32_t m_texturecount = 0;

	uint32_t CreateTextureHandle();
	void CreateTextureImage(uint32_t handle, int width, int height);
	void CreateTextureImage(uint32_t handle, const char* imagesrc);
	void CreateTextureImageView(uint32_t handle);
	void AddImageToTexture(uint32_t handle, const char* imagesrc);

	void CleanTextures();


	//Vertex buffer modulation

	std::vector<Buffer> mh_vertexbuffers;

	uint32_t m_vertexbuffercount = 0;
	uint32_t CreateVertexBufferHandle();
	void CreateVertexBuffer(uint32_t handle, const void* buffer, size_t elementsize, uint32_t elementcount);
	void CleanVertexBuffers();

	//Index buffer modulation
	

	std::vector<Buffer> mh_indexbuffers;

	uint32_t m_indexbuffercount = 0;
	uint32_t CreateIndexBufferHandle();
	void CreateIndexBuffer(uint32_t handle, void* buffer, uint32_t indexcount);

	void CleanIndexBuffers();

	//Uniform buffer modulation
	std::vector<UniformBuffer> mh_uniformbuffers;

	uint32_t m_uniformbuffercount = 0;

	uint32_t CreateUniformBufferHandle();
	void CreateUniformBuffer(uint32_t handle, size_t buffersize);
	void UpdateUniformBuffer(uint32_t handle, const void* buffer, const uint32_t currentframe);

	void CleanUniformBuffers();

	//Descriptor pool modulation
	uint32_t m_descriptorpoolcount = 0;
	std::vector<DescriptorPool> mh_descriptorpools;


	uint32_t CreateDescriptorPoolHandle();
	void AddDescriptorPoolSize(uint32_t handle, VkDescriptorType type);
	void CreateDescriptorPool(uint32_t handle);

	void CleanDescriptorPools();

	//Descriptor set modulation
	uint32_t m_descriptorsetcount = 0;
	std::unordered_map<uint32_t, DescriptorSet>mh_descriptorsets;
	

	uint32_t CreateDescriptorSetHandle();
	void CreateDescriptorSets(uint32_t handle, uint32_t layouthandle, uint32_t poolhandle);
	WriteDescriptorSet* CreateWriteDescriptorSet(uint32_t handle, uint32_t descriptorcount, uint32_t bindingidx, VkDescriptorType descriptortype, uint32_t* writedescriptorindex);
	void AddDescriptorBufferInfoToWriteDescriptorSet(uint32_t handle, uint32_t writedescriptorindex, uint32_t uniformbufferhandle, size_t offset, size_t range);
	void AddDescriptorImageInfoToWriteDescriptorSet(uint32_t handle, uint32_t writedescriptorindex,VkImageLayout imagelayout, VkImageView imageview, VkSampler sampler);



	/// <Command Buffer Modulation>
	
	uint32_t m_commandbuffercount = 0;
	std::unordered_map<uint32_t, std::vector<VkCommandBuffer>> mh_commandbuffers;
	
	uint32_t CreateCommandBufferHandle();
	void CreateCommandBuffer(uint32_t handle, VkCommandPool& commandpool, VkCommandBufferLevel level);


	//Record Command Buffer modulation

	void RecordCommandBuffer(VkCommandBuffer& commandbuffer, const uint32_t& swapchainimageindex, const uint32_t graphicspipelinehandle, const uint32_t vertexbufferhandle, const uint32_t indexbufferhandle, const uint32_t descriptorsethandle, const uint32_t renderpasshandle);

	
	//Vulkan memory allocator

	VmaAllocatorCreateInfo m_vmaalloccreateinfo{};
	VmaAllocator m_vmaallocator;
	VmaVulkanFunctions m_vmafunctions;

	void CreateAllocator();
	void CleanAllocator();


	const std::vector<const char*> m_deviceextensions
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};


	//test 
	ModelViewProjectionBuffer buf;
	void MVP()
	{
		static auto starttime = std::chrono::high_resolution_clock::now();

		auto currenttime = std::chrono::high_resolution_clock::now();

		float time = std::chrono::duration<float, std::chrono::seconds::period>(currenttime - starttime).count();

		ModelViewProjectionBuffer mvp;

		mvp.model = glm::mat4(1.0);
		mvp.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0f, 0.0f, 1.0f));
		mvp.projection = glm::perspective(45.f, ((float)m_swapchainextent.width / (float)m_swapchainextent.height), 0.1f, 100.f);

		mvp.projection[1][1] *= -1;
		buf = mvp;
	}
	//test\\\

	//quarantine functions

	void CreateImageT(const uint32_t& width, const uint32_t height, VkFormat format, VkImageTiling tiling, const VkImageUsageFlags& usage, const VkMemoryPropertyFlags& properties, VkImage& image, VmaAllocation& allocation, VkSharingMode sharingmode = VK_SHARING_MODE_EXCLUSIVE);



private: // Member variables




private:
	//bool IsDeviceSuitable(VkPhysicalDevice& physicaldevice);
	
	
	uint32_t m_currentframe = 0;



	const std::vector<Vertex> vertices = {
	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
	{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	};
	const std::vector<uint32_t> indices =
	{
		0, 1, 2, 2, 3, 0

	};


	const std::vector<Vertex> TESTvertices = {
	{{-0.1f, -0.1f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.1f, -0.1f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.1f, 0.1f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
	{{-0.1f, 0.1f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	};

	
	
	


};