/*****************************************************************//**
 * @file   RenderPass.h
 * @brief  Function declarations for render pass creation, initialization, finalization and destruction.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/

#pragma once
#include "../Core/PersistanceVkCore.h"
#include "../Structures/RenderPass.h"

namespace PersistanceBackend {
	
	
	VkSubpassDescription createSubpassDescription(const AttachmentReferenceList* colorAttachments, const RenderPassAttachment* depthAndStencilAttachment, const AttachmentReferenceList* inputAttachments, const uint32_t* preserveAttachmentIndices, const uint32_t preserveAttachmentCount);

	VkSubpassDependency createSubpassDependency(uint32_t srcSubpass, uint32_t dstSubpass, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags);

	VkRenderPass createRenderPass(const VkSubpassDescription* subpasses, const uint32_t subpassCount, const VkSubpassDependency* subpassDependency, const uint32_t dependencyCount, const AttachmentDescriptionList& renderPassAttachments);

	void beginRenderPass(VkCommandBuffer& commandBuffer, VkRenderPass& renderPass, Framebuffer& framebuffer, VkOffset2D offset, VkExtent2D extent, const VkClearValue* clearValues, const uint32_t clearValueCount, const uint32_t* currentImage = nullptr);

	void endRenderPass(VkCommandBuffer& commandBuffer);

	void cleanUpRenderPasses(const VkRenderPass* renderPass, uint32_t count);


};

