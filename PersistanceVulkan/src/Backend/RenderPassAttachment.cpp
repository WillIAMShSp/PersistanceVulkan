/*****************************************************************//**
 * @file   RenderPassAttachment.cpp
 * @brief  Function definition for render pass attachment creation.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/

#include "RenderPassAttachment.h"


/**
 * @brief Creates a render pass attachment.
 * 
 * @param attachmentIndex The index of the attachment.
 * @param attachmentLayout The layout of the attachment.
 * @param format The format of the attachment.
 * @param imagesamples The amount of samples for the attachment.
 * @param loadop The attachment's load operation.
 * @param storeop The attachment's store operation.
 * @param initialimagelayout The attachment's initial image layout.
 * @param finalimagelayout The attachment's final image layout.
 * @return A render pass attachment.
 */
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
