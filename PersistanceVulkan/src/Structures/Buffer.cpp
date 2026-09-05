#include "./Buffer.h"
#include "Vendor/vma/VmaUsage.h"


Buffer PersistanceBackend::createBuffer(const VkDeviceSize& size, VkBufferUsageFlags usageflags, VkMemoryPropertyFlags properties, VkSharingMode sharingmode)
{

	Buffer buffer;

	PersistanceUtils::createBuffer(size, usageflags, properties, buffer.buffer, buffer.allocation);
	
	return buffer;
}

void PersistanceBackend::mapBuffer(Buffer &buffer, void *dataMap) {
  
	vmaMapMemory(core.getAllocator(), buffer.allocation, &dataMap);

}

