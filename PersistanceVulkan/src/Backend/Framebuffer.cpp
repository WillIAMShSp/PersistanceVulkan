#include "Framebuffer.h"

#include "../Core/CoreUtils.h"
/**
 * creates as many framebuffers as the max frames in flight and assigns them to an object.
 * 
 * @param renderpass the renderpass this framebuffer will be made for.
 * @param width the width of the framebuffer image.
 * @param height the height of the framebuffer image.
 * @param layers the layers of the framebuffer
 * @param imageFormat the format of the framebuffer image.
 * @param imageTiling the tiling operation for the framebuffer image.
 * @param imageUsageFlags how the framebuffer image will be used.
 * @param imageMemoryProperties the framebuffer image's memory properties.
 * @return the fully created framebuffers
 */
Framebuffer PersistanceBackend::createFramebuffer(VkRenderPass& renderpass, uint32_t width, uint32_t height, uint32_t layers, VkFormat imageFormat, VkImageTiling imageTiling, VkImageUsageFlags imageUsageFlags, VkMemoryPropertyFlags imageMemoryProperties)
{
	Framebuffer framebuffer;

	uint32_t imageCount = PersistanceLib::MAXFRAMESINFLIGHT;

	framebuffer.images.resize(imageCount);
	framebuffer.allocations.resize(imageCount);
	framebuffer.imageviews.resize(imageCount);
	framebuffer.framebuffers.resize(imageCount);

	for (int i = 0; i < imageCount; i++) 
	{
		PersistanceUtils::createImage(width, height, imageFormat, imageTiling, imageUsageFlags, imageMemoryProperties, framebuffer.images[i], framebuffer.allocations[i], VK_SHARING_MODE_CONCURRENT, VK_IMAGE_LAYOUT_UNDEFINED);
		
		framebuffer.imageviews[i] = PersistanceUtils::createImageView(framebuffer.images[i], imageFormat);

		VkFramebufferCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.width = width;
		info.height = height;
		info.attachmentCount = 1;
		info.pAttachments = &framebuffer.imageviews[i];
		info.layers = layers;
		info.renderPass = renderpass;
		
		vkCreateFramebuffer(core.m_device, &info, nullptr, &framebuffer.framebuffers[i]);
	
	}
	return framebuffer;

	
}
