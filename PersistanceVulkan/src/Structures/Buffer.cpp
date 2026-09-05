#include "./Buffer.h"


Buffer createBuffer(const VkDeviceSize& size, VkBufferUsageFlags usageflags, VkMemoryPropertyFlags properties, VkSharingMode sharingmode)
{

	Buffer buffer;

	PersistanceUtils::createBuffer(size, usageflags, properties, buffer.buffer, buffer.allocation);
	
	return buffer;
}


