/*****************************************************************//**
 * @file   GraphicsPipeline.cpp
 * @brief  Function definitions for graphics pipeline creation and destruction.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/

#include "GraphicsPipeline.h"


/**
 * @brief Creates a pipeline layout.
 * Breaks if the pipeline layout creation is unsuccessful.
 * 
 * @param layouts Descriptor set layouts assigned to the pipeline.
 * @param layoutCount The amount of descriptor set layouts.
 * @param pushConstantRanges is a pointer to an array of VkPushConstantRange structures defining a set of push constant ranges for use in a single pipeline layout. In addition to descriptor set layouts, 
 * a pipeline layout also describes how many push constants can be accessed by each stage of the pipeline.
 * @param pushConstangRangeCount Is the number of push constant ranges included in the pipeline layout
 * @return A pipeline layout.
 */
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

	if (vkCreatePipelineLayout(core.getDevice(), &info, nullptr, &layout) != VK_SUCCESS) 
	{
		BREAK(0);
	}

	return layout;
}

/**
 * @brief Creates a graphics pipeline.
 * Breaks if graphics pipeline creation is unsuccessful.
 * 
 * @param layout The pipeline's layout.
 * @param shader The shader associated with the created graphics pipeline.
 * @param settings The pipeline settings.
 * @param renderPass The renderpass associated with the created graphics pipeline.
 * @return A VkPipeline with the created graphics pipeline.
 */
VkPipeline PersistanceBackend::createGraphicsPipeline(VkPipelineLayout& layout, Shader& shader, PipelineSettings& settings, VkRenderPass& renderPass)
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


	pipelinecreateinfo.pVertexInputState = &settings.getVertexInputStateCreateInfo();
	pipelinecreateinfo.pInputAssemblyState = &settings.getInputAssemblyStateCreateInfo();
	pipelinecreateinfo.pViewportState = &settings.getViewportCreateInfo();
	pipelinecreateinfo.pRasterizationState = &settings.getRasterCreateInfo();
	pipelinecreateinfo.pMultisampleState = &settings.getMultisampleCreateInfo();
	pipelinecreateinfo.pColorBlendState = &settings.getColorBlendCreateInfo();
	pipelinecreateinfo.pDepthStencilState = settings.getDepthStencilCreateInfo();


	pipelinecreateinfo.layout = layout;
	pipelinecreateinfo.pDynamicState = (settings.m_usedynamicstate) ? &dynamicstatecreateinfo : nullptr;
	pipelinecreateinfo.renderPass = renderPass;
	pipelinecreateinfo.subpass = 0;

	pipelinecreateinfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelinecreateinfo.basePipelineIndex = -1;

	pipelinecreateinfo.pNext = nullptr;

	if (vkCreateGraphicsPipelines(core.getDevice(), VK_NULL_HANDLE, 1, &pipelinecreateinfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create the graphics pipeline");

	}

	for (auto modules : shader.shaderModules) 
	{
		vkDestroyShaderModule(core.getDevice(), modules, nullptr);
	}

	return graphicsPipeline;

}

/**
 * @brief Destroys provided pipelines.
 * 
 * @param pipelines 
 * @param pipelineCount
 */
void PersistanceBackend::cleanUpGraphicsPipeline(VkPipeline* pipelines, const uint32_t pipelineCount)
{
	for (uint32_t i = 0; i < pipelineCount; i++) 
	{
		vkDestroyPipeline(core.getDevice(), pipelines[i], nullptr);
	}

}

/**
 * @brief Destroys provided pipeline layouts.
 * 
 * @param layouts
 * @param layoutCount
 */
void PersistanceBackend::cleanUpPipelineLayouts(VkPipelineLayout* layouts, const uint32_t layoutCount)
{
	for (uint32_t i = 0; i < layoutCount; i++) 
	{
		vkDestroyPipelineLayout(core.getDevice(), layouts[i], nullptr);
	}
}
