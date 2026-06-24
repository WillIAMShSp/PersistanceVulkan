#pragma once
#include "../Core/PersistanceVkCore.h"
#include "../Structures/RenderPass.h"

namespace PersistanceBackend {
	
	
	VkSubpassDescription createSubpassDescription(const AttachmentReferenceList* colorAttachments, const RenderPassAttachment* depthAndStencilAttachment, const AttachmentReferenceList* inputAttachments, const uint32_t* preserveAttachmentIndices, const uint32_t preserveAttachmentCount);

	VkSubpassDependency createSubpassDependency(uint32_t srcSubpass, uint32_t dstSubpass, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags);

	RenderPass createRenderPass(const VkSubpassDescription* subpasses, const uint32_t subpassCount, const VkSubpassDependency* subpassDependency, const uint32_t dependencyCount, const AttachmentDescriptionList& renderPassAttachments);

	void beginRenderPass(VkCommandBuffer& commandBuffer, RenderPass& renderPass, Framebuffer& framebuffer, VkOffset2D offset, VkExtent2D extent, VkClearValue clearValue);

	void endRenderPass(VkCommandBuffer& commandBuffer);

	void cleanUpRenderPasses(const RenderPass* renderPass, uint32_t count);


};

