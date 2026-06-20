#include "RenderPass.h"

/**
 * creates a subpass description.
 * 
 * \param colorAttachments the list of type AttachmentReferenceList of the subpass color attachment references
 * \param depthAndStencilAttachment the depth and stencil attachment
 * \param inputAttachments the list of type AttachmentReferenceList of the subpass input attachment references
 * \param preserveAttachmentIndices the list of type const int* of preserve attachment indices
 * \param preserveAttachmentCount the amount of attachment indices
 * \return a subpass description.
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
 * Creates a subpass dependency.
 * 
 * \param srcSubpass which subpass comes before this dependency (example: the first subpass)
 * \param dstSubpass which subpass comes after this dependency (example: second subpass)
 * \param srcAccessMask 
 * \param dstAccessMask 
 * \param srcStageMask
 * \param dstStageMask
 * \param dependencyFlags
 * \return 
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
 * Creates a renderpass.
 * 
 * \param subpasses the subpass descriptions
 * \param subpassCount the amount of subpasses
 * \param subpassDependency
 * \param dependencyCount
 * \param renderPassAttachments
 * \return 
 */
RenderPass PersistanceBackend::createRenderPass(const VkSubpassDescription* subpasses, const uint32_t subpassCount, const VkSubpassDependency* subpassDependency, const uint32_t dependencyCount, const AttachmentDescriptionList& renderPassAttachments)
{
	RenderPass renderPassObject;
	VkRenderPass& renderPass = renderPassObject.renderpass;
	

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

	return renderPassObject;
}

void PersistanceBackend::cleanUpRenderPasses(const RenderPass* renderPass, uint32_t count)
{
	for (int i = 0; i < count; i++) {
		vkDestroyRenderPass(core.m_device, renderPass[i].renderpass, nullptr);

	}
}

