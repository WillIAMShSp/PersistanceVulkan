#include "Buffer.h"
#include "../Core/PersistanceVkCore.h"

Buffer createBuffer(const VkDeviceSize& size, VkBufferUsageFlags usageflags, VkMemoryPropertyFlags properties, VkSharingMode sharingmode)
{

	Buffer buffer;


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


	if (vmaCreateBuffer(core.m_vmaAllocator, &buffercreateinfo, &allocationcreateinfo, &buffer.buffer, &buffer.allocation, nullptr))
	{
		std::cout << "Failed to create buffer";
		BREAK(0);

	}
	return buffer;
}
