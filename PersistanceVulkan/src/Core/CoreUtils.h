#include "./PersistanceVkCore.h"


namespace PersistanceUtils {

	void createBuffer(const VkDeviceSize& size,
		VkBufferUsageFlags usageflags,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VmaAllocation& allocation,
		VkSharingMode sharingmode = VK_SHARING_MODE_EXCLUSIVE);

	void createImage(
		const uint32_t& width, 
		const uint32_t height, 
		VkFormat format, 
		VkImageTiling tiling, 
		const VkImageUsageFlags& usage, 
		const VkMemoryPropertyFlags& properties, 
		VkImage& image, 
		VmaAllocation& allocation, 
		VkSharingMode sharingmode, 
		VkImageLayout initiallayout);

	void copyBuffer(VkBuffer& srcbuffer, VkBuffer& dstbuffer, VkDeviceSize size, VkCommandPool& commandpool, VkQueue& submitqueue, VkDeviceSize srcoffset = VkDeviceSize(0), VkDeviceSize dstoffset = VkDeviceSize(0));
	void copyBufferToImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height, VkCommandPool& commandpool, VkQueue& submitqueue);
	void transitionImageLayout(VkImage& image, const VkFormat& format, VkImageLayout oldlayout, VkImageLayout newlayout, VkCommandPool& commandpool, VkQueue submitqueue);
	
	VkImageMemoryBarrier createImageMemoryBarrier
	(
		VkImage& image, 
		VkImageLayout oldLayout, 
		VkImageLayout newLayout, 
		VkAccessFlags srcAccessMask, 
		VkAccessFlags dstAccessMask, 
		VkImageSubresourceRange subresourceRange,
		uint32_t srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED
	);


	VkCommandBuffer beginSingleTimeCommands(VkCommandPool& commandpool, const VkCommandBufferLevel& level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	void endSingleTimeCommands(VkCommandBuffer& commandbuffer, const VkCommandPool& commandpool, const VkQueue& submitqueue);

	std::vector<char> readFile(const char* filepath);
	VkShaderModule createShaderModule(std::vector<char>& shaderFile);

	VkImageView createImageView(VkImage& image, VkFormat format, VkImageAspectFlags imageaspect = VK_IMAGE_ASPECT_COLOR_BIT);

	VkIndexType findIndexType(size_t indexBufferElementSize);

	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat findDepthFormat();
	bool hasStencilComponent(VkFormat format);

}
