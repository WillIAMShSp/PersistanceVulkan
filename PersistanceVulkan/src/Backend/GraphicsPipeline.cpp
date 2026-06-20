#include "GraphicsPipeline.h"



VkPipelineLayout PersistanceBackend::createPipelineLayout(const VkDescriptorSetLayout* layouts, const uint32_t layoutCount, const VkPushConstantRange* pushConstantRanges, const uint32_t pushConstangRangeCount)
{
	
	VkPipelineLayout layout;

	VkPipelineLayoutCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	info.pPushConstantRanges = pushConstantRanges;
	info.pushConstantRangeCount = pushConstangRangeCount;
	info.pSetLayouts = layouts;
	info.setLayoutCount = layoutCount;
	info.flags = 0;

	if (vkCreatePipelineLayout(core.m_device, &info, nullptr, &layout) != VK_SUCCESS) 
	{
		BREAK(0);
	}

	return layout;
}

VkPipeline PersistanceBackend::createGraphicsPipeline(VkPipelineLayout& layout, Shader& shader, PipelineSettings& settings, RenderPass& renderPass)
{
	VkPipeline graphicsPipeline;


	std::vector<VkDynamicState> dynamicstates =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR

	};

	VkPipelineDynamicStateCreateInfo dynamicstatecreateinfo{};
	dynamicstatecreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicstatecreateinfo.dynamicStateCount = static_cast<uint32_t>(dynamicstates.size());
	dynamicstatecreateinfo.pDynamicStates = dynamicstates.data();


	//////////////////////////////////
	VkGraphicsPipelineCreateInfo pipelinecreateinfo{};


	pipelinecreateinfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelinecreateinfo.stageCount = static_cast<uint32_t>(shader.stages.size());
	pipelinecreateinfo.pStages = shader.stages.data();


	pipelinecreateinfo.pVertexInputState = &settings.GetVertexInputStateCreateInfo();
	pipelinecreateinfo.pInputAssemblyState = &settings.GetInputAssemblyStateCreateInfo();
	pipelinecreateinfo.pViewportState = &settings.Getviewportcreateinfo();
	pipelinecreateinfo.pRasterizationState = &settings.GetRasterCreateInfo();
	pipelinecreateinfo.pMultisampleState = &settings.GetMultisampleCreateInfo();
	pipelinecreateinfo.pColorBlendState = &settings.GetColorBlendCreateInfo();
	pipelinecreateinfo.pDepthStencilState = nullptr;


	pipelinecreateinfo.layout = layout;
	pipelinecreateinfo.pDynamicState = (settings.m_usedynamicstate) ? &dynamicstatecreateinfo : nullptr;
	pipelinecreateinfo.renderPass = renderPass.renderpass;
	pipelinecreateinfo.subpass = 0;

	pipelinecreateinfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelinecreateinfo.basePipelineIndex = -1;

	pipelinecreateinfo.pNext = nullptr;

	if (vkCreateGraphicsPipelines(core.m_device, VK_NULL_HANDLE, 1, &pipelinecreateinfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create the graphics pipeline");

	}



}
