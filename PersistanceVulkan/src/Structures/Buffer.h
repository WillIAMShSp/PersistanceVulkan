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
	void* mapBuffer(Buffer& buffer);
	void cleanUpBuffers(Buffer* buffers, uint32_t bufferCount);
	
	template<size_t N>
	std::array<VkBuffer, N> viewBufferArray(const Buffer(&buffers)[N]) 
	{
		std::array<VkBuffer, N> bufferArray;
		for (size_t i = 0; i < N; i++) 
		{
			bufferArray[i] = buffers[i].buffer;
		}
		return bufferArray;
	}
}
	