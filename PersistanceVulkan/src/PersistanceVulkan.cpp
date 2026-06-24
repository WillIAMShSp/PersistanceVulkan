
#include "PersistanceVk.h"


#include "./Backend/RenderPass.h";
#include "./Backend/RenderPassAttachment.h"
#include "./Backend/DescriptorSetLayout.h"
#include "./Backend/GraphicsPipeline.h"
#include "./Backend/TextureSampler.h"
#include "./Backend/Texture.h"
#include "./Backend/VertexBuffer.h"
#include "./Backend/IndexBuffer.h"
#include "./Backend/DescriptorPool.h"
#include "./Backend/UniformBuffer.h"
#include "./Backend/DescriptorSet.h"
#include "./Backend/Framebuffer.h"
#include "./Backend/CommandBuffer.h"



struct Vertex
{
	glm::vec2 position;
	glm::vec3 color;
	glm::vec2 uv;


};

PersistanceVk engine;



ModelViewProjectionBuffer buf;
void MVP()
{
	static auto starttime = std::chrono::high_resolution_clock::now();

	auto currenttime = std::chrono::high_resolution_clock::now();

	float time = std::chrono::duration<float, std::chrono::seconds::period>(currenttime - starttime).count();

	ModelViewProjectionBuffer mvp;

	mvp.model = glm::mat4(1.0);
	mvp.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0f, 0.0f, 1.0f));
	mvp.projection = glm::perspective(45.f, ((float)core.m_swapchainExtent.width) / ((float)core.m_swapchainExtent.height), 0.1f, 100.f);

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


/*const std::vector<float> vertices = {
-0.1f, -0.1f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
0.1f, -0.1f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
0.1f, 0.1f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
-0.1f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
};*/

Drawable drawing;







int main() {
    
	core.init();
	

	RenderPassAttachment attachment = PersistanceBackend::createRenderPassAttachment(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, core.m_swapchainImageFormat, 
		VK_SAMPLE_COUNT_1_BIT, 
		VK_ATTACHMENT_LOAD_OP_CLEAR, 
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	
	AttachmentReferenceList refList;
	refList.add(attachment);
	AttachmentDescriptionList disList;
	disList.add(&attachment, 1);

	VkSubpassDescription description = PersistanceBackend::createSubpassDescription(&refList, nullptr, nullptr, nullptr, 0);

	VkSubpassDependency dependency = PersistanceBackend::createSubpassDependency(VK_SUBPASS_EXTERNAL, 0, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0);
	
	RenderPass renderpass = PersistanceBackend::createRenderPass(&description, 1, &dependency, 1, disList);

	VkDescriptorSetLayoutBinding bindings[2] = {
	PersistanceBackend::createDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
	PersistanceBackend::createDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)

	};
	VkDescriptorSetLayout layout = PersistanceBackend::createDescriptorSetLayout(&bindings[0], 2);


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

	PersistanceBackend::Shader shader;
	shader.createShaderStage("res/Shaders/basicvert.spv", VK_SHADER_STAGE_VERTEX_BIT);
	shader.createShaderStage("res/Shaders/basicfrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);



	VkPipelineLayout pipelayout = PersistanceBackend::createPipelineLayout(&layout, 1, nullptr, 0);
	VkPipeline graphicsPipeline = PersistanceBackend::createGraphicsPipeline(pipelayout, shader, settings, renderpass);

	
	Texture texture = PersistanceBackend::createTexture("res/Textures/Placeholder.png");
	VkSampler sampler = PersistanceBackend::createTextureSampler();

	Buffer vertexBuffer = PersistanceBackend::createVertexBuffer(vertices.data(), sizeof(vertices[0]), vertices.size());
	Buffer indexBuffer = PersistanceBackend::createIndexBuffer(indices.data(), sizeof(indices[0]), indices.size());

	
	DescriptorPoolSizeList desPoolList;
	
	VkDescriptorPoolSize uniformBufferSize = PersistanceBackend::createDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	VkDescriptorPoolSize samplerSize = PersistanceBackend::createDescriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

	desPoolList.add(&uniformBufferSize, 1);
	desPoolList.add(&samplerSize, 1);

	VkDescriptorPool descriptorPool = PersistanceBackend::createDescriptorPool(desPoolList);

	UniformBuffer uniformBuffer = PersistanceBackend::createUniformBuffer(sizeof(ModelViewProjectionBuffer));
	PersistanceBackend::updateUniformBuffers(uniformBuffer, &buf, sizeof(ModelViewProjectionBuffer));


	VkWriteDescriptorSet writeDescriptors[2];
	std::vector<VkDescriptorSet> descriptorSet = PersistanceBackend::allocateDescriptorSet(descriptorPool, PersistanceLib::MAXFRAMESINFLIGHT, layout);
	std::vector<VkDescriptorBufferInfo> uniformBufferinfo = PersistanceBackend::createDescriptorBufferInfo(uniformBuffer.buffers.data(), uniformBuffer.buffers.size(), 0, sizeof(ModelViewProjectionBuffer));
	writeDescriptors[0] = PersistanceBackend::createWriteDescriptorSet(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, uniformBufferinfo.data(), nullptr, 0);
	std::vector<VkDescriptorImageInfo> imageInfo = PersistanceBackend::createDescriptorImageInfo(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, texture.imageview, sampler);
	writeDescriptors[1] = PersistanceBackend::createWriteDescriptorSet(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, nullptr, imageInfo.data(), 0);

	PersistanceBackend::updateDescriptorSets(descriptorSet, writeDescriptors, 2);

	Framebuffer framebuffer = PersistanceBackend::createFramebuffer(renderpass.renderpass, screenwidth, screenheight, 1, VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VkCommandBuffer commandBuffer = PersistanceBackend::allocateCommandBuffer(core.m_graphicsCommandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	while (core.isRunning()) 
	{
		core.pollEvents();

		




	}
	core.waitForDeviceIdle();


 //   engine.Init();


	//uint32_t renderpasshandle = engine.CreateRenderPassHandle();
	//uint32_t colorattachment = engine.CreateRenderPassColorAttachment(
	//	renderpasshandle,
	//	engine.GetSwapchainImageFormat(),
	//	VK_SAMPLE_COUNT_1_BIT,
	//	VK_ATTACHMENT_LOAD_OP_CLEAR,
	//	VK_ATTACHMENT_STORE_OP_STORE,
	//	VK_IMAGE_LAYOUT_UNDEFINED,
	//	VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	//uint32_t subpassdescription = engine.CreateSubpassDescription(renderpasshandle, &colorattachment, 1, UINT32_MAX, nullptr, 0, nullptr, 0);
	//uint32_t subpassdependency = engine.CreateSubpassDependency(
	//	renderpasshandle,
	//	VK_SUBPASS_EXTERNAL,
	//	0,
	//	VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	//	VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	//	0,
	//	VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
	//engine.CreateRenderPass(renderpasshandle, &colorattachment, 1, &subpassdescription, 1, &subpassdependency, 1);
	//

	//engine.CreateSwapchainFramebufferImageViews();
	//engine.CreateSwapchainFramebuffers(renderpasshandle);
	//

	//
	//int descriptorsetlayouthandle;
	//descriptorsetlayouthandle = engine.CreateDescriptorSetLayoutHandle();
	//engine.AddDescriptorSetLayoutBinding(descriptorsetlayouthandle, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	//engine.AddDescriptorSetLayoutBinding(descriptorsetlayouthandle, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

	//engine.CreateDescriptorSetLayout(descriptorsetlayouthandle);

	//uint32_t pipeline = engine.CreateGraphicsPipelineHandle();

	//engine.AddVertexStage(pipeline, "res/Shaders/basicvert.spv"); /*Create shaders*/
	//engine.AddFragmentStage(pipeline, "res/Shaders/basicfrag.spv"); //
	//engine.CreateGraphicsPipelineLayout(pipeline, descriptorsetlayouthandle);
	//PipelineSettings settings;
	//VertexInputStateLayout vertexbufferlayout;
	//vertexbufferlayout.push<glm::vec2>();/*Configure vertex array layout*/
	//vertexbufferlayout.push<glm::vec3>();//
	//vertexbufferlayout.push<glm::vec2>();//
	//settings.CreateVertexInputState(vertexbufferlayout);
	//settings.DefineInputAssemblyState();
	//settings.CreateStaticViewPort();
	//settings.ConfigureRasterizationStage();
	//settings.ConfigureMultisample();
	//settings.ConfigureColorBlend();
	//settings.UseDynamicViewport();
	//engine.CreateGraphicsPipeline(pipeline, settings, renderpasshandle);

	//
	//
	//uint32_t texturesamplerhandle = engine.CreateTextureSamplerHandle();
	//uint32_t texturehandle = engine.CreateTextureHandle();
	//engine.CreateTextureImage(texturehandle, "res/Textures/Placeholder.png");
	//engine.CreateTextureImageView(texturehandle);
	//engine.CreateTextureSampler(texturesamplerhandle);

	//uint32_t vertexbufferhndl = engine.CreateVertexBufferHandle();
	//engine.CreateVertexBuffer(vertexbufferhndl, vertices.data(), sizeof(vertices[0]), (uint32_t)vertices.size());

	//uint32_t indexbufferhndl = engine.CreateIndexBufferHandle();
	//engine.CreateIndexBuffer(indexbufferhndl, (void*)indices.data(), (uint32_t)indices.size());

	//uint32_t uniformbufferhandle = engine.CreateUniformBufferHandle();
	//engine.CreateUniformBuffer(uniformbufferhandle, sizeof(ModelViewProjectionBuffer));

	//uint32_t descriptorpoolhandle = engine.CreateDescriptorPoolHandle();
	//engine.AddDescriptorPoolSize(descriptorpoolhandle, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	//engine.AddDescriptorPoolSize(descriptorpoolhandle, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
	//engine.CreateDescriptorPool(descriptorpoolhandle);

	//uint32_t descriptorsethandle = engine.CreateDescriptorSetHandle();
	//uint32_t uniformbufferwritedescriptor;
	//uint32_t texturewritedescriptor;
	//engine.CreateWriteDescriptorSet(descriptorsethandle, 1, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &uniformbufferwritedescriptor);
	//engine.AddDescriptorBufferInfoToWriteDescriptorSet(descriptorsethandle, uniformbufferwritedescriptor, uniformbufferhandle, 0, sizeof(ModelViewProjectionBuffer));
	//engine.CreateWriteDescriptorSet(descriptorsethandle, 1, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &texturewritedescriptor);
	//engine.AddDescriptorImageInfoToWriteDescriptorSet(descriptorsethandle, texturewritedescriptor, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, texturehandle, texturesamplerhandle);
	//engine.CreateDescriptorSets(descriptorsethandle, descriptorsetlayouthandle, descriptorpoolhandle);

	//uint32_t commandbufferhandle = engine.CreateCommandBufferHandle();
	//engine.CreateCommandBuffer(commandbufferhandle, engine.GetGraphicsCommandPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	//Drawable drawing;
	//drawing.AddVertexBuffer(vertexbufferhndl);
	//drawing.AddVertexBufferOffset(0);
	//drawing.SetDescriptorSetHandle(descriptorsethandle);
	//drawing.SetGraphicsPipelineHandle(pipeline);
	//drawing.SetIndexBufferHandle(indexbufferhndl);
	//drawing.SetGraphicsPipelineBindingPoint(VK_PIPELINE_BIND_POINT_GRAPHICS);

    //while (engine.IsRunning()) 
 //   {
 //       engine.PollEvents();

	//	engine.StartDrawing(0, 0);

	//	MVP();
	//	engine.UpdateUniformBuffer(uniformbufferhandle, &buf, engine.GetCurrentFrame());

	//	engine.BeginCommandBuffer(commandbufferhandle, 0);

	//	VkClearValue clearcolor = { {{0.f, 0.f, 0.f, 1.0f}} };
	//	VkOffset2D offset = { 0,0 };

	//

	//	//engine.TransitionImageLayout(framebuffer, engine.GetCurrentFrame(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	//	


	//	engine.BeginRenderPass(commandbufferhandle, renderpasshandle, true, 0, clearcolor, offset, engine.GetSwapchainExtent());
	//	engine.BindGraphicsPipeline(pipeline, VK_PIPELINE_BIND_POINT_GRAPHICS, 0);
	//	engine.SetViewport(commandbufferhandle, 0.f, 0.f, 0.f, 1.f, engine.GetSwapchainExtent());
	//	engine.SetScissors(commandbufferhandle, offset, engine.GetSwapchainExtent());
	//	engine.DrawIndexed(commandbufferhandle, drawing);
	//	engine.EndRenderPass(commandbufferhandle);
	//	

	//	



	//	engine.EndCommandBuffer(commandbufferhandle);
	//	engine.EndAndPresentDrawing(commandbufferhandle, renderpasshandle);


 //       
 //   }
 //   engine.WaitForDeviceIdle();
 //   engine.End();
 //   
 //   
 //   return 0;
}