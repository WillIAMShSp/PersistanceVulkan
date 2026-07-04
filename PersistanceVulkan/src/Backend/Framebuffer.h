/*****************************************************************//**
 * @file   Framebuffer.h
 * @brief  Function declaration for framebuffer creation and destruction.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/
#include "../Structures/Framebuffer.h"
#include "../PersistanceLib.h"

namespace PersistanceBackend {

	Framebuffer createFramebuffer(VkRenderPass& renderpass, 
		uint32_t width, 
		uint32_t height, 
		uint32_t layers, 
		VkFormat imageFormat, 
		VkImageTiling imageTiling, 
		VkImageUsageFlags imageUsageFlags, 
		VkMemoryPropertyFlags imageMemoryProperties,
		VkImageView* depthAndStencilImageView = nullptr
	);
	
	void cleanUpFramebuffers(Framebuffer* framebuffers, const uint32_t framebufferCount);

}

