#include "../Structures/Framebuffer.h"
#include "../PersistanceLib.h"

namespace PersistanceBackend {

	Framebuffer createFramebuffer(VkRenderPass& renderpass, uint32_t width, uint32_t height, uint32_t layers, VkFormat imageFormat, VkImageTiling imageTiling, VkImageUsageFlags imageUsageFlags, VkMemoryPropertyFlags imageMemoryProperties);
	


}

