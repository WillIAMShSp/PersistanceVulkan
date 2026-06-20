#include "RenderPassAttachment.h"



RenderPassAttachment PersistanceBackend::createRenderPassAttachment(const uint32_t& attachmentIndex, VkImageLayout attachmentLayout, VkFormat format, VkSampleCountFlagBits imagesamples, VkAttachmentLoadOp loadop, VkAttachmentStoreOp storeop, VkImageLayout initialimagelayout, VkImageLayout finalimagelayout)
{
	RenderPassAttachment attachment;
	attachment.reference.layout = attachmentLayout;
	attachment.reference.attachment = attachmentIndex;

	attachment.description.flags = 0;
	attachment.description.format = format;
	attachment.description.samples = imagesamples;
	attachment.description.loadOp = loadop;
	attachment.description.storeOp = storeop;
	attachment.description.initialLayout = initialimagelayout;
	attachment.description.finalLayout = finalimagelayout;

	attachment.description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;


	return attachment;

}
