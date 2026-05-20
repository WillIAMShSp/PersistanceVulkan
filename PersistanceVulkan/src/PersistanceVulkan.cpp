
#include "PersistanceVk.h"



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
	mvp.projection = glm::perspective(45.f, ((float)engine.GetSwapchainExtent().width / (float)engine.GetSwapchainExtent().height), 0.1f, 100.f);

	mvp.projection[1][1] *= -1;
	buf = mvp;
}
//test\\\


//const std::vector<Vertex> vertices = {
//{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
//{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
//{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
//{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
//};
const std::vector<uint32_t> indices =
{
	0, 1, 2, 2, 3, 0

};


const std::vector<float> vertices = {
-0.1f, -0.1f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
0.1f, -0.1f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
0.1f, 0.1f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
-0.1f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
};

Drawable drawing;







int main() {
    
    engine.Init();


	uint32_t renderpasshandle = engine.CreateRenderPassHandle();
	uint32_t colorattachment = engine.CreateRenderPassColorAttachment(
		renderpasshandle,
		engine.GetSwapchainImageFormat(),
		VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	uint32_t subpassdescription = engine.CreateSubpassDescription(renderpasshandle, &colorattachment, 1, UINT32_MAX, nullptr, 0, nullptr, 0);
	uint32_t subpassdependency = engine.CreateSubpassDependency(
		renderpasshandle,
		VK_SUBPASS_EXTERNAL,
		0,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		0,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
	engine.CreateRenderPass(renderpasshandle, &colorattachment, 1, &subpassdescription, 1, &subpassdependency, 1);
	engine.CreateSwapchainFramebuffers(renderpasshandle);
	
	int descriptorsetlayouthandle;
	descriptorsetlayouthandle = engine.CreateDescriptorSetLayoutHandle();
	engine.AddDescriptorSetLayoutBinding(descriptorsetlayouthandle, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	engine.AddDescriptorSetLayoutBinding(descriptorsetlayouthandle, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

	engine.CreateDescriptorSetLayout(descriptorsetlayouthandle);

	uint32_t pipeline = engine.CreateGraphicsPipelineHandle();

	engine.AddVertexStage(pipeline, "res/Shaders/basicvert.spv"); /*Create shaders*/
	engine.AddFragmentStage(pipeline, "res/Shaders/basicfrag.spv"); //
	engine.CreateGraphicsPipelineLayout(pipeline, descriptorsetlayouthandle);
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
	engine.CreateGraphicsPipeline(pipeline, settings, renderpasshandle);
	
	uint32_t texturesamplerhandle = engine.CreateTextureSamplerHandle();
	uint32_t texturehandle = engine.CreateTextureHandle();
	engine.CreateTextureImage(texturehandle, "res/Textures/Placeholder.png");
	engine.CreateTextureImageView(texturehandle);
	engine.CreateTextureSampler(texturesamplerhandle);

	uint32_t vertexbufferhndl = engine.CreateVertexBufferHandle();
	engine.CreateVertexBuffer(vertexbufferhndl, vertices.data(), sizeof(vertices[0]), (uint32_t)vertices.size());

	uint32_t indexbufferhndl = engine.CreateIndexBufferHandle();
	engine.CreateIndexBuffer(indexbufferhndl, (void*)indices.data(), (uint32_t)indices.size());

	uint32_t uniformbufferhandle = engine.CreateUniformBufferHandle();
	engine.CreateUniformBuffer(uniformbufferhandle, sizeof(ModelViewProjectionBuffer));

	uint32_t descriptorpoolhandle = engine.CreateDescriptorPoolHandle();
	engine.AddDescriptorPoolSize(descriptorpoolhandle, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	engine.AddDescriptorPoolSize(descriptorpoolhandle, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
	engine.CreateDescriptorPool(descriptorpoolhandle);

	uint32_t descriptorsethandle = engine.CreateDescriptorSetHandle();
	uint32_t uniformbufferwritedescriptor;
	uint32_t texturewritedescriptor;
	engine.CreateWriteDescriptorSet(descriptorsethandle, 1, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &uniformbufferwritedescriptor);
	engine.AddDescriptorBufferInfoToWriteDescriptorSet(descriptorsethandle, uniformbufferwritedescriptor, uniformbufferhandle, 0, sizeof(ModelViewProjectionBuffer));
	engine.CreateWriteDescriptorSet(descriptorsethandle, 1, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &texturewritedescriptor);
	engine.AddDescriptorImageInfoToWriteDescriptorSet(descriptorsethandle, texturewritedescriptor, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, texturehandle, texturesamplerhandle);
	engine.CreateDescriptorSets(descriptorsethandle, descriptorsetlayouthandle, descriptorpoolhandle);

	uint32_t commandbufferhandle = engine.CreateCommandBufferHandle();
	engine.CreateCommandBuffer(commandbufferhandle, engine.GetGraphicsCommandPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	Drawable drawing;
	drawing.AddVertexBuffer(vertexbufferhndl);
	drawing.AddVertexBufferOffset(0);
	drawing.SetDescriptorSetHandle(descriptorsethandle);
	drawing.SetGraphicsPipelineHandle(pipeline);
	drawing.SetIndexBufferHandle(indexbufferhndl);
	drawing.SetGraphicsPipelineBindingPoint(VK_PIPELINE_BIND_POINT_GRAPHICS);

    while (engine.IsRunning()) 
    {
        engine.PollEvents();

		engine.StartDrawing(0, 0);

		MVP();
		engine.UpdateUniformBuffer(uniformbufferhandle, &buf, engine.GetCurrentFrame());

		engine.BeginCommandBuffer(commandbufferhandle, 0);

		VkClearValue clearcolor = { {{0.f, 0.f, 0.f, 1.0f}} };
		VkOffset2D offset = { 0,0 };

		engine.BeginRenderPass(commandbufferhandle, renderpasshandle, clearcolor, VkRect2D(), engine.GetSwapchainExtent());
		engine.BindGraphicsPipeline(pipeline, VK_PIPELINE_BIND_POINT_GRAPHICS, 0);
		engine.SetViewport(commandbufferhandle, 0.f, 0.f, 0.f, 1.f, engine.GetSwapchainExtent());
		engine.SetScissors(commandbufferhandle, offset, engine.GetSwapchainExtent());
		engine.DrawIndexed(commandbufferhandle, drawing);
		engine.EndRenderPass(commandbufferhandle);
		engine.EndCommandBuffer(commandbufferhandle);
		engine.EndAndPresentDrawing(commandbufferhandle, renderpasshandle);


        
    }
    engine.WaitForDeviceIdle();
    engine.End();
    
    
    return 0;
}