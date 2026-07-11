/*****************************************************************//**
 * @file   RenderPass.cpp
 * @brief  Function definitions for subpass description, subpass dependency and renderpass creation, initialization, finalization and destruction.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/
#include "RenderPass.h"


/**
 * @brief Creates a subpass description.
 * 
 * @param colorAttachments the list of type AttachmentReferenceList of the subpass color attachment references
 * @param depthAndStencilAttachment the depth and stencil attachment
 * @param inputAttachments the list of type AttachmentReferenceList of the subpass input attachment references
 * @param preserveAttachmentIndices the list of type const int* of preserve attachment indices
 * @param preserveAttachmentCount the amount of attachment indices
 * @return A subpass description.
 */
VkSubpassDescription PersistanceBackend::createSubpassDescription(const AttachmentReferenceList* colorAttachments, const RenderPassAttachment* depthAndStencilAttachment, const AttachmentReferenceList* inputAttachments, const uint32_t* preserveAttachmentIndices, const uint32_t preserveAttachmentCount)
{
	VkSubpassDescription description{};


	description.flags = 0;
	description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

	if (colorAttachments != nullptr) 
	{
		description.pColorAttachments = colorAttachments->references.data();
		description.colorAttachmentCount = colorAttachments->references.size();
	}
	
	
	description.pDepthStencilAttachment = (depthAndStencilAttachment != nullptr) ? &depthAndStencilAttachment->reference : nullptr;
	
	if (inputAttachments != nullptr)
	{
		description.pInputAttachments = inputAttachments->references.data();
		description.inputAttachmentCount = inputAttachments->references.size();
	}

	description.pPreserveAttachments = preserveAttachmentIndices;
	description.preserveAttachmentCount = preserveAttachmentCount;

	return description;



}



/**
 * @brief Creates a subpass dependency.
 * 
 * @param srcSubpass Which subpass comes before this dependency (example: the first subpass)
 * @param dstSubpass Which subpass comes after this dependency (example: second subpass)
 * @param srcAccessMask Is a bitmask of VkAccessFlagBits specifying a source access mask.
 * @param dstAccessMask Is a bitmask of VkAccessFlagBits specifying a destination access mask.
 * @param srcStageMask  Is a bitmask of VkPipelineStageFlagBits specifying the source stage mask. 
 * If set to VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, it is equivalent to setting it to VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT.
 * @param dstStageMask Is a bitmask of VkPipelineStageFlagBits specifying the destination stage mask.
 * If set to VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, it is equivalent to setting it to VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT.
 * @param dependencyFlags
 * @return 
 */
VkSubpassDependency PersistanceBackend::createSubpassDependency(uint32_t srcSubpass, uint32_t dstSubpass, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags)
{
	VkSubpassDependency dependency{};

	dependency.srcSubpass = srcSubpass;
	dependency.dstSubpass = dstSubpass;
	dependency.srcAccessMask = srcAccessMask;
	dependency.dstAccessMask = dstAccessMask;
	dependency.srcStageMask = srcStageMask;
	dependency.dstStageMask = dstStageMask;
	dependency.dependencyFlags = dependencyFlags;
	
	return dependency;
}




/**
 * @brief Creates a render pass.
 * Breaks if renderpass creation is unsuccessful.
 * 
 * @param subpasses The subpass descriptions
 * @param subpassCount The amount of subpasses
 * @param subpassDependency An array of subpass dependencies.
 * @param dependencyCount The amount of subpass dependencies.
 * @param renderPassAttachments A list of renderpass attachment descriptions.
 * @return A renderpass.
 */
VkRenderPass PersistanceBackend::createRenderPass(const VkSubpassDescription* subpasses, const uint32_t subpassCount, const VkSubpassDependency* subpassDependency, const uint32_t dependencyCount, const AttachmentDescriptionList& renderPassAttachments)
{
	VkRenderPass renderPass;

	VkRenderPassCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	info.pNext = nullptr;

	info.pSubpasses = subpasses;
	info.subpassCount = subpassCount;
	info.pDependencies = subpassDependency;
	info.dependencyCount = dependencyCount;
	info.pAttachments = renderPassAttachments.descriptions.data();
	info.attachmentCount = renderPassAttachments.descriptions.size();



	info.flags = 0;

	if (vkCreateRenderPass(core.m_device, &info, nullptr, &renderPass) != VK_SUCCESS) {
		BREAK(0);
	}

	return renderPass;
}


/**
 * @brief Begins a specified render pass on a specified command buffer.
 * 
 * @param commandBuffer The command buffer to begin the render pass on.
 * @param renderPass The render pass initialized.
 * @param framebuffer The render pass framebuffer
 * @param offset The offset of the render pass area.
 * @param extent The extent of the render pass area.
 * @param clearValues The render pass clear values, one per attachment. 
 * Note, clear values must be in the same order as their attachments in the renderpass.
 * @param clearValueCount the amount of clear values
 */
void PersistanceBackend::beginRenderPass(VkCommandBuffer& commandBuffer, VkRenderPass& renderPass, Framebuffer& framebuffer, VkOffset2D offset, VkExtent2D extent, const VkClearValue* clearValues, const uint32_t clearValueCount, const uint32_t* currentImage)
{
	uint32_t imageIndex = currentImage ? *currentImage : core.m_imageIndex;

	VkRenderPassBeginInfo info{};
	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	info.clearValueCount = clearValueCount;
	info.pClearValues = clearValues;
	info.framebuffer = framebuffer.framebuffers[imageIndex];
	info.renderArea.extent = extent;
	info.renderArea.offset = offset;
	info.renderPass = renderPass;
	
	vkCmdBeginRenderPass(commandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
}

/**
 * @brief Ends a renderpass on the specified command bufffer.
 * 
 * @param commandBuffer Command buffer specified.
 */
void PersistanceBackend::endRenderPass(VkCommandBuffer& commandBuffer)
{
	vkCmdEndRenderPass(commandBuffer);
}

/**
 * @brief Cleans up provided render passes.
 * 
 * @param renderPass the render passes provided.
 * @param count The amount of render passes provided.
 */
void PersistanceBackend::cleanUpRenderPasses(const VkRenderPass* renderPass, uint32_t count)
{
	for (int i = 0; i < count; i++) {
		vkDestroyRenderPass(core.m_device, renderPass[i], nullptr);

	}
}

