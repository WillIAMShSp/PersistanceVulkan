#include "./Buffer.h"
#include "Vendor/vma/VmaUsage.h"


Buffer PersistanceBackend::createBuffer(const VkDeviceSize& size, VkBufferUsageFlags usageflags, VkMemoryPropertyFlags properties, VkSharingMode sharingmode)
{

	Buffer buffer;

	PersistanceUtils::createBuffer(size, usageflags, properties, buffer.buffer, buffer.allocation, VK_SHARING_MODE_CONCURRENT);
	
	return buffer;
}

void *PersistanceBackend::mapBuffer(Buffer &buffer)
{
	void* dataMap;
	vmaMapMemory(core.getAllocator(), buffer.allocation, &dataMap);
    return dataMap;
}

void PersistanceBackend::cleanUpBuffers(Buffer *buffers, uint32_t bufferCount)
{
	for (uint32_t i = 0; i < bufferCount; i++) 
	{
		vmaDestroyBuffer(core.getAllocator(), buffers[i].buffer, buffers[i].allocation);
	}
}
