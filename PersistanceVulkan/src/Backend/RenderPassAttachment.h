/*****************************************************************//**
 * @file   RenderPassAttachment.h
 * @brief  Function declaration for render pass attachment creation.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/
#pragma once

#include "../Structures/RenderPassAttachment.h"

namespace PersistanceBackend {

	RenderPassAttachment createRenderPassAttachment(const uint32_t& attachmentIndex, VkImageLayout attachmentLayout, VkFormat format, VkSampleCountFlagBits imagesamples, VkAttachmentLoadOp loadop, VkAttachmentStoreOp storeop, VkImageLayout initialimagelayout, VkImageLayout finalimagelayout);

}
