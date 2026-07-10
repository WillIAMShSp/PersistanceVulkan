#include "CoreUtils.h"


/**
 * @brief Creates and allocates a Vulkan buffer.
 * 
 * @param size the size of the buffer
 * @param usageflags defines the buffer usage
 * @param properties the buffer's memory properties
 * @param buffer the buffer to be created
 * @param allocation the buffer allocation
 * @param sharingmode sharing mode used for the buffer's creation, if VK_SHARING_MODE_CONCURRENT, the transfer family queue is used
 */
void PersistanceUtils::createBuffer(const VkDeviceSize& size, VkBufferUsageFlags usageflags, VkMemoryPropertyFlags properties, VkBuffer& buffer, VmaAllocation& allocation, VkSharingMode sharingmode)
{
	VkBufferCreateInfo buffercreateinfo{};
	buffercreateinfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffercreateinfo.size = size;

	buffercreateinfo.usage = usageflags;
	buffercreateinfo.sharingMode = sharingmode;
	std::array<uint32_t, 2> queuefamilyindices;
	if (sharingmode & VK_SHARING_MODE_CONCURRENT)
	{
		queuefamilyindices =
		{
			core.m_queueFamilyIndices.graphicsfamily,
			core.m_queueFamilyIndices.transferfamily

		};

		buffercreateinfo.queueFamilyIndexCount = static_cast<uint32_t>(queuefamilyindices.size());
		buffercreateinfo.pQueueFamilyIndices = queuefamilyindices.data();

	}


	VmaAllocationCreateInfo allocationcreateinfo{};
	allocationcreateinfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocationcreateinfo.minAlignment = 0;
	allocationcreateinfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
		VMA_ALLOCATION_CREATE_MAPPED_BIT;


	if (vmaCreateBuffer(core.m_vmaAllocator, &buffercreateinfo, &allocationcreateinfo, &buffer, &allocation, nullptr))
	{
		std::cout << "Failed to create buffer";
		BREAK(0);

	}

}

/**
 * @brief Creates an image with the provided arguments.
 * 
 * @param width The image width.
 * @param height The image height.
 * @param format The image format.
 * @param tiling The image tiling i.e. Linear or Optimal.
 * @param usage How the image will be used.
 * @param properties Image memory properties.
 * @param image The image created.
 * @param allocation The image allocation.
 * @param sharingmode The sharing mode.
 * @param initiallayout The image initial layout.
 */
void PersistanceUtils::createImage(const uint32_t& width, const uint32_t height, VkFormat format, VkImageTiling tiling, const VkImageUsageFlags& usage, const VkMemoryPropertyFlags& properties, VkImage& image, VmaAllocation& allocation, VkSharingMode sharingmode, VkImageLayout initiallayout)
{
	VkImageCreateInfo imageinfo{};
	imageinfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageinfo.imageType = VK_IMAGE_TYPE_2D;
	imageinfo.extent.width = width;
	imageinfo.extent.height = height;
	imageinfo.extent.depth = 1;
	imageinfo.mipLevels = 1;
	imageinfo.arrayLayers = 1;
	imageinfo.format = format;
	imageinfo.tiling = tiling;
	imageinfo.initialLayout = initiallayout;
	imageinfo.usage = usage;
	imageinfo.sharingMode = sharingmode;
	imageinfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageinfo.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT; //temporary solution, This means that the framebuffer image can have the same format as a texture, but change to that of the swapchain

	std::array<uint32_t, 2> queuefamilyindices;									  // so I can also write to it, or thats my theory at least, lets give it a go for now.

	if (sharingmode & VK_SHARING_MODE_CONCURRENT)
	{
		queuefamilyindices =
		{
			core.m_queueFamilyIndices.graphicsfamily,
			core.m_queueFamilyIndices.transferfamily

		};

		imageinfo.queueFamilyIndexCount = static_cast<uint32_t>(queuefamilyindices.size());
		imageinfo.pQueueFamilyIndices = queuefamilyindices.data();



	}
	else
	{
		imageinfo.queueFamilyIndexCount = 1;
		imageinfo.pQueueFamilyIndices = &core.m_queueFamilyIndices.graphicsfamily;
	}

	VmaAllocationCreateInfo allocationcreateinfo{};
	allocationcreateinfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocationcreateinfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
	allocationcreateinfo.priority = 1.0;



	if (vmaCreateImage(core.m_vmaAllocator, &imageinfo, &allocationcreateinfo, &image, &allocation, nullptr) != VK_SUCCESS)
	{
		std::cout << "Failed to create image!";
		BREAK(0);
	}
}

/**
 * @brief Copies a source buffer into a destination buffer, where both buffers live in the GPU.
 * 
 * @param srcbuffer The source buffer.
 * @param dstbuffer The destination buffer.
 * @param size The size of the region of the source buffer to be copied.
 * @param commandpool The command pool from which to execute this copy.
 * @param submitqueue The submit queue submiting this execution.
 * @param srcoffset The offset of the start of the data copied from the source buffer.
 * @param dstoffset The offset of the start of the copied region in the destination buffer.
 */
void PersistanceUtils::copyBuffer(VkBuffer& srcbuffer, VkBuffer& dstbuffer, VkDeviceSize size, VkCommandPool& commandpool, VkQueue& submitqueue, VkDeviceSize srcoffset, VkDeviceSize dstoffset)
{
	VkCommandBuffer commandbuffer = beginSingleTimeCommands(commandpool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);


	VkBufferCopy copyregion{};
	copyregion.srcOffset = srcoffset;
	copyregion.dstOffset = dstoffset;
	copyregion.size = size;
	vkCmdCopyBuffer(commandbuffer, srcbuffer, dstbuffer, 1, &copyregion);

	endSingleTimeCommands(commandbuffer, commandpool, submitqueue);

}

/**
 * @brief Copies a buffer to an image.
 * 
 * @param buffer The source buffer.
 * @param image The destination image.
 * @param width The image width.
 * @param height The image height.
 * @param commandpool The command pool from which this copy is executed.
 * @param submitqueue The submit queue submitting this execution.
 */
void PersistanceUtils::copyBufferToImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height, VkCommandPool& commandpool, VkQueue& submitqueue)
{
	VkCommandBuffer commandbuffer = beginSingleTimeCommands(commandpool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);


	VkBufferImageCopy imgcopy{};
	imgcopy.bufferOffset = 0;
	imgcopy.bufferImageHeight = 0;
	imgcopy.bufferRowLength = 0;

	imgcopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imgcopy.imageSubresource.baseArrayLayer = 0;
	imgcopy.imageSubresource.mipLevel = 0;
	imgcopy.imageSubresource.layerCount = 1;

	imgcopy.imageOffset = { 0, 0, 0 };
	imgcopy.imageExtent = { width, height, 1 };


	vkCmdCopyBufferToImage(commandbuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imgcopy);

	endSingleTimeCommands(commandbuffer, commandpool, submitqueue);
}

void PersistanceUtils::transitionImageLayout(VkImage& image, const VkFormat& format, VkImageLayout oldlayout, VkImageLayout newlayout, VkCommandPool& commandpool, VkQueue submitqueue)
{
	VkCommandBuffer commandbuffer = beginSingleTimeCommands(commandpool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);


	VkPipelineStageFlags srcstage;
	VkPipelineStageFlags dststage;



	VkImageMemoryBarrier membarrier{};
	membarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	membarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	membarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	membarrier.oldLayout = oldlayout;
	membarrier.newLayout = newlayout;

	membarrier.image = image;
	membarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	membarrier.subresourceRange.baseArrayLayer = 0;
	membarrier.subresourceRange.layerCount = 1;
	membarrier.subresourceRange.baseMipLevel = 0;
	membarrier.subresourceRange.levelCount = 1;

	if (oldlayout == VK_IMAGE_LAYOUT_UNDEFINED && newlayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{

		membarrier.srcAccessMask = 0;
		membarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		srcstage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dststage = VK_PIPELINE_STAGE_TRANSFER_BIT;


	}
	else if (oldlayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newlayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		membarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		membarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		srcstage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dststage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;


	}
	else if (oldlayout == VK_IMAGE_LAYOUT_UNDEFINED && newlayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		membarrier.srcAccessMask = 0;
		membarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		srcstage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dststage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

	}
	else
	{
		throw std::invalid_argument("Layout unsupported!");

	}


	vkCmdPipelineBarrier(
		commandbuffer,
		srcstage, //todo
		dststage, //todo
		0, 0,
		nullptr, 0,
		nullptr, 1,
		&membarrier
	);

	endSingleTimeCommands(commandbuffer, commandpool, submitqueue);

}



VkCommandBuffer PersistanceUtils::beginSingleTimeCommands(VkCommandPool& commandpool, const VkCommandBufferLevel& level)
{
	VkCommandBufferAllocateInfo allocinfo{};
	allocinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocinfo.commandBufferCount = 1;
	allocinfo.commandPool = commandpool;
	allocinfo.level = level;

	VkCommandBuffer commandbuffer;

	if (vkAllocateCommandBuffers(core.m_device, &allocinfo, &commandbuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate command buffer");
	}

	VkCommandBufferBeginInfo begininfo{};
	begininfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begininfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandbuffer, &begininfo);



	return commandbuffer;
}

void PersistanceUtils::endSingleTimeCommands(VkCommandBuffer& commandBuffer, const VkCommandPool& commandpool, const VkQueue& submitqueue)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitinfo{};
	submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitinfo.commandBufferCount = 1;
	submitinfo.pCommandBuffers = &commandBuffer;


	vkQueueSubmit(submitqueue, 1, &submitinfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(submitqueue);

	vkFreeCommandBuffers(core.m_device, commandpool, 1, &commandBuffer);
}

std::vector<char> PersistanceUtils::readFile(const char* filepath)
{
	std::ifstream file(filepath, std::ios::ate | std::ios::binary);

	;
	if (!file.is_open())
	{
		std::cout << "Could not find file at: " << filepath << "\n";


		throw std::runtime_error("failed to read the file");

	}


	size_t filesize = (size_t)file.tellg();

	std::vector<char> buffer(filesize);


	file.seekg(0);
	file.read(buffer.data(), filesize);

	file.close();

	return buffer;



}

VkShaderModule PersistanceUtils::createShaderModule(std::vector<char>& shaderFile)
{
	VkShaderModule module;

	VkShaderModuleCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	info.codeSize = shaderFile.size();
	info.pCode = reinterpret_cast<const uint32_t*>(shaderFile.data());
	info.flags = 0;
	if (vkCreateShaderModule(core.m_device, &info, nullptr, &module) != VK_SUCCESS) 
	{
		BREAK(0);
	}

	return module;


}

VkImageView PersistanceUtils::createImageView(VkImage& image, VkFormat format, VkImageAspectFlags imageaspect)
{
	VkImageView imageview;

	VkImageViewCreateInfo viewinfo{};
	viewinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewinfo.image = image;
	viewinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewinfo.format = format;
	viewinfo.subresourceRange.aspectMask = imageaspect;
	viewinfo.subresourceRange.baseArrayLayer = 0;
	viewinfo.subresourceRange.layerCount = 1;
	viewinfo.subresourceRange.baseMipLevel = 0;
	viewinfo.subresourceRange.levelCount = 1;

	if (vkCreateImageView(core.m_device, &viewinfo, nullptr, &imageview) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create texture image view");

	}


	return imageview;
}

VkIndexType PersistanceUtils::findIndexType(size_t indexBufferElementSize)
{
	switch (indexBufferElementSize) {

		case sizeof(uint32_t) :
			return VK_INDEX_TYPE_UINT32;
			break;

			case sizeof(uint16_t) :
				return VK_INDEX_TYPE_UINT16;

				case sizeof(uint8_t) :
					return VK_INDEX_TYPE_UINT8;

				default:
					std::cout << "\nIndex Type not Supported!";
					BREAK;



	};

	return VK_INDEX_TYPE_NONE_KHR;
}



/**
 * @brief Finds a supported image format out of a pool of candidates with specified features depending on tiling and on the selected physical device.
 * 
 * Breaks if no supported image format found.
 * 
 * @param candidates A pool of format candidates.
 * @param tiling The tiling mode of the requiring image.
 * @param features Are the features the format must support to be eligible.
 * @return The supported format.
 */
VkFormat PersistanceUtils::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (auto candidate : candidates) {
		VkFormatProperties formatProperties;

		vkGetPhysicalDeviceFormatProperties(core.m_physicalDevice, candidate, &formatProperties);

		if (tiling == VK_IMAGE_TILING_LINEAR && (features & formatProperties.linearTilingFeatures) == features) {
			return candidate;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (features & formatProperties.optimalTilingFeatures) == features) {
			return candidate;
		}
	}

	std::cout << "\nNo supported Format found\n";
	BREAK;
	return VkFormat();
}


/**
 * @brief Finds the most appropiate depth format for the selected physical device.
 * 
 * @return The depth format.
 */
VkFormat PersistanceUtils::findDepthFormat()
{
	return findSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

/**
 * @brief Evaluates wether or not a provided depth format supports the stencil component.
 * 
 * @param format 
 * @return True if the provided format does support the stencil component, false otherwise.
 */
bool PersistanceUtils::hasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}
