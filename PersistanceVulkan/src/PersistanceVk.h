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
#include "Structures/Drawable.h"






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
	uint32_t presentfamily = -1;
	uint32_t computefamily = -1;
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










class PersistanceVk
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
		CreateSyncObjects();
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
		uint32_t subpassdescription = CreateSubpassDescription(renderpasshandle, &colorattachment, 1, UINT32_MAX, nullptr, 0, nullptr, 0);
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



		//////// DescriptorSetLayouts

		int descriptorsetlayouthandle;
		descriptorsetlayouthandle = CreateDescriptorSetLayoutHandle();
		AddDescriptorSetLayoutBinding(descriptorsetlayouthandle, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		AddDescriptorSetLayoutBinding(descriptorsetlayouthandle, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

		CreateDescriptorSetLayout(descriptorsetlayouthandle);

		//The way this works is we create a layout handle and we create bindings associating it to that handle. we create a descriptorsetlayout associated with that handle. For now, it becomes the used descriptorsetlayout.

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
		
		FrameBufferHandle frmbffrhndl = CreateFrameBuffersHandle();
		CreateFramebufferImage(frmbffrhndl);
		CreateFramebufferImageViews(frmbffrhndl);
		CreateFramebuffers(frmbffrhndl, renderpasshandle);

		
				//CreateTextureSampler();
		uint32_t texturesamplerhandle = CreateTextureSamplerHandle();
		uint32_t texturehandle = CreateTextureHandle();
		CreateTextureImage(texturehandle, "res/Textures/Placeholder.png");
		CreateTextureImageView(texturehandle);
		CreateTextureSampler(texturesamplerhandle);

		
				
		uint32_t vertexbufferhndl = CreateVertexBufferHandle();
		CreateVertexBuffer(vertexbufferhndl, vertices.data(), sizeof(vertices[0]), (uint32_t)vertices.size());

		/////TESTVB/////
		uint32_t testvbhandl = CreateVertexBufferHandle();
		CreateVertexBuffer(testvbhandl, TESTvertices.data(), sizeof(TESTvertices[0]), (uint32_t)TESTvertices.size());
		////////////////


		uint32_t indexbufferhndl = CreateIndexBufferHandle();
		CreateIndexBuffer(indexbufferhndl, (void*)indices.data(), (uint32_t)indices.size());

				//CreateUniformBuffer();

		uint32_t uniformbufferhandle = CreateUniformBufferHandle();
		CreateUniformBuffer(uniformbufferhandle, sizeof(ModelViewProjectionBuffer));

				//CreateDescriptorPool();

		uint32_t descriptorpoolhandle = CreateDescriptorPoolHandle();
		AddDescriptorPoolSize(descriptorpoolhandle, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		AddDescriptorPoolSize(descriptorpoolhandle, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		CreateDescriptorPool(descriptorpoolhandle);

				//CreateDescriptorSets();

		uint32_t descriptorsethandle = CreateDescriptorSetHandle();
		uint32_t uniformbufferwritedescriptor;
		uint32_t texturewritedescriptor;
		CreateWriteDescriptorSet(descriptorsethandle, 1, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &uniformbufferwritedescriptor);
		AddDescriptorBufferInfoToWriteDescriptorSet(descriptorsethandle, uniformbufferwritedescriptor, uniformbufferhandle, 0, sizeof(ModelViewProjectionBuffer));
		CreateWriteDescriptorSet(descriptorsethandle, 1, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &texturewritedescriptor);
		AddDescriptorImageInfoToWriteDescriptorSet(descriptorsethandle, texturewritedescriptor, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, texturehandle, texturesamplerhandle);
		CreateDescriptorSets(descriptorsethandle, descriptorsetlayouthandle, descriptorpoolhandle);

		////////////////////////////////////
				//CreateCommandBuffer();
				//////////////////////
		uint32_t commandbufferhandle = CreateCommandBufferHandle();
		CreateCommandBuffer(commandbufferhandle, m_graphicscommandpool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);



		drawing.AddVertexBuffer(vertexbufferhndl);
		drawing.AddVertexBufferOffset(0);
		drawing.SetDescriptorSetHandle(descriptorsethandle);
		drawing.SetGraphicsPipelineHandle(pipeline);
		drawing.SetIndexBufferHandle(indexbufferhndl);
		drawing.SetGraphicsPipelineBindingPoint(VK_PIPELINE_BIND_POINT_GRAPHICS);

	}

	void MainLoop()
	{
		while (!glfwWindowShouldClose(m_window))
		{
			glfwPollEvents();


			


			StartDrawing(0,0);

			MVP();
			UpdateUniformBuffer(0, &buf, GetCurrentFrame());

			BeginCommandBuffer(0,GetCurrentFrame(), 0);

			VkClearValue clearcolor = { {{0.f, 0.f, 0.f, 1.0f}} };
			VkOffset2D offset = { 0,0 };

			BeginRenderPass(0, GetCurrentFrame(), 0, m_imageindex, clearcolor, VkRect2D(), GetSwapchainExtent());
			BindGraphicsPipeline(0,GetCurrentFrame(), VK_PIPELINE_BIND_POINT_GRAPHICS, 0);
			SetViewport(0, GetCurrentFrame(), 0, 0, 0.f, 1.f, GetSwapchainExtent());
			SetScissors(0,GetCurrentFrame(), offset, GetSwapchainExtent());
			DrawIndexed(0,GetCurrentFrame(), drawing);
			EndRenderPass(0, GetCurrentFrame());
			EndCommandBuffer(0, GetCurrentFrame());
			EndAndPresentDrawing(0, 0);
			
			//DrawFrame(0,0,0,0,0,0);

			

		}

		vkDeviceWaitIdle(m_device);

	}

	void CleanUp()
	{


		CleanUpSwapchain();


		CleanTextures();
		
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
		auto app = reinterpret_cast<PersistanceVk*>(glfwGetWindowUserPointer(window));
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
		VkSharingMode sharingmode = VK_SHARING_MODE_EXCLUSIVE);

	void CopyBuffer(VkBuffer& srcbuffer, VkBuffer& dstbuffer, VkDeviceSize size, VkCommandPool& commandpool, VkQueue& submitqueue);

	uint32_t FindMemoryType(uint32_t typefilter, VkMemoryPropertyFlags flags);

	void TransitionImageLayout(VkImage& image, const VkFormat& format, VkImageLayout oldlayout, VkImageLayout newlayout, VkCommandPool& commandpool, VkQueue submitqueue);

	void CopyBuffertoImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height, VkCommandPool& commandpool, VkQueue& submitqueue);

	VkImageView CreateImageView(VkImage& image, VkFormat format, VkImageAspectFlags imageaspect = VK_IMAGE_ASPECT_COLOR_BIT);

private: //member variables

	GLFWwindow* m_window = nullptr;
	VkInstance m_instance;
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

	uint32_t m_currentframe = 0;
	uint32_t m_imageindex = 0;
	bool m_drawingstarted = false;

	uint32_t m_renderpasscount = 0;
	std::vector<RenderPass> mh_renderpasses;
	std::vector<DescriptorSetLayout> mh_descriptorsetlayouts;
	uint32_t m_dslhandlecount = 0;
	uint32_t m_pipelinecount = 0;
	std::vector<GraphicsPipeline> mh_graphicspipelines;
	uint32_t m_framebuffercount = 0;
	std::vector<Framebuffer> mh_framebuffers;
	std::vector<Texture> mh_textures;
	std::vector <VkSampler> mh_texturesamplers;
	uint32_t m_texturecount = 0;
	uint32_t m_texturesamplercount = 0;
	std::vector<Buffer> mh_vertexbuffers;
	uint32_t m_vertexbuffercount = 0;
	std::vector<Buffer> mh_indexbuffers;
	uint32_t m_indexbuffercount = 0;
	std::vector<UniformBuffer> mh_uniformbuffers;
	uint32_t m_uniformbuffercount = 0;
	uint32_t m_descriptorpoolcount = 0;
	std::vector<DescriptorPool> mh_descriptorpools;
	uint32_t m_descriptorsetcount = 0;
	std::vector<DescriptorSet>mh_descriptorsets;
	uint32_t m_commandbuffercount = 0;
	std::vector<std::vector<VkCommandBuffer>> mh_commandbuffers;
	VmaAllocatorCreateInfo m_vmaalloccreateinfo{};
	VmaAllocator m_vmaallocator;
	VmaVulkanFunctions m_vmafunctions;
	
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


	//Drawing
	void DrawFrame(const uint32_t commandbufferhandle, const uint32_t graphicspipelinehandle, const uint32_t vertexbufferhandle, const uint32_t indexbufferhandle, const uint32_t descriptorsethandle, const uint32_t renderpasshandle);

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
	uint32_t CreateRenderpassAttachment(RenderPassHandle handle,
		VkImageLayout attachmentlayout,
		VkFormat format,
		VkSampleCountFlagBits imagesamples,
		VkAttachmentLoadOp loadop,
		VkAttachmentStoreOp storeop,
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

	FrameBufferHandle CreateFrameBuffersHandle();

	void CreateFramebufferImage(FrameBufferHandle handle);
	void CreateFramebufferImage(FrameBufferHandle handle, int width, int height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageflags, VkMemoryPropertyFlags memoryproperties);
	void CreateFramebufferImageViews(FrameBufferHandle handle);
	void CreateFramebuffers(FrameBufferHandle handle, const uint32_t renderpasshandle);

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


	// Record Command Buffer modulation

	void RecordCommandBuffer(VkCommandBuffer& commandbuffer, const uint32_t& swapchainimageindex, const GraphicsPipelineHandle graphicspipelinehandle, const BufferHandle vertexbufferhandle, const BufferHandle indexbufferhandle, const DescriptorSetHandle descriptorsethandle, const RenderPassHandle renderpasshandle);


	void StartDrawing(BufferHandle commandbufferhandle, RenderPassHandle renderpasshandle);
	void BeginCommandBuffer(BufferHandle commandbufferhandle, uint32_t currentframe, VkCommandBufferUsageFlags flags);
	void BeginRenderPass(BufferHandle commandbufferhandle, uint32_t commandbufferframe, RenderPassHandle renderpasshandle, uint32_t swapchainimageindex, VkClearValue clearcolor, VkRect2D offset, VkExtent2D extent);
	void BindGraphicsPipeline(BufferHandle commandbufferhandle, uint32_t commandbufferframe, VkPipelineBindPoint bindingpoint, GraphicsPipelineHandle handle);
	void SetViewport(BufferHandle commandbufferhandle, uint32_t commandbufferframe, float xpos, float ypos, float mindepth, float maxdepth, VkExtent2D extent);
	void SetScissors(BufferHandle commandbufferhandle, uint32_t commandbufferframe, VkOffset2D offset, VkExtent2D extent);
	void Draw(BufferHandle commandbufferhandle, uint32_t commandbufferframe, Drawable drawsettings);
	void DrawIndexed(BufferHandle commandbufferhandle, uint32_t commandbufferframe, Drawable drawsettings);
	void EndRenderPass(BufferHandle commandbufferhandle, uint32_t commandbufferframe);
	void EndCommandBuffer(BufferHandle commandbufferhandle, uint32_t commandbufferframe);
	void EndAndPresentDrawing(BufferHandle commandbufferhandle, RenderPassHandle renderpassHandle);

	//Vulkan memory allocator

	void CreateAllocator();
	void CleanAllocator();

private: // Tesing
	//bool IsDeviceSuitable(VkPhysicalDevice& physicaldevice);


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

	Drawable drawing;

};