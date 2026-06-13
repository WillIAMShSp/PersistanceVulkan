#pragma once

#include "../Structures/RenderPassAttachment.h"


RenderPassAttachment createRenderPassAttachment(const uint32_t& attachmentIndex, VkImageLayout attachmentLayout, VkFormat format, VkSampleCountFlagBits imagesamples, VkAttachmentLoadOp loadop, VkAttachmentStoreOp storeop, VkImageLayout initialimagelayout, VkImageLayout finalimagelayout);

