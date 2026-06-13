#pragma once
#include "../Core/PersistanceVkCore.h"
#include "../Structures/RenderPass.h"


VkSubpassDescription createSubpassDescription(const AttachmentReferenceList& colorAttachments, const RenderPassAttachment& depthAndStencilAttachment, const AttachmentReferenceList& inputAttachments, const uint32_t* preserveAttachmentIndices, const uint32_t preserveAttachmentCount);

VkSubpassDependency createSubpassDependency(uint32_t srcSubpass, uint32_t dstSubpass, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags);

RenderPass createRenderPass();
