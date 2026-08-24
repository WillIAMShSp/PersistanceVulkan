#include "./Pipeline.h"

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

