#pragma once
#include "../PersistanceLib.h"
#include "Core/CoreUtils.h"

typedef uint32_t BufferHandle;

struct Buffer {
	Buffer() = default;

	VkBuffer buffer;
	VmaAllocation allocation;
	size_t size;
	size_t elementSize;
};

namespace PersistanceBackend 
{
	Buffer createBuffer(const VkDeviceSize& size, VkBufferUsageFlags usageflags, VkMemoryPropertyFlags properties, VkSharingMode sharingmode = VK_SHARING_MODE_EXCLUSIVE);
	void mapBuffer(Buffer& buffer, void* dataMap);
}
	