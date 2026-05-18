#pragma once
#include "PersistanceLib.h"

typedef uint32_t BufferHandle;

struct Buffer {
	Buffer() = default;

	VkBuffer buffer;
	VmaAllocation allocation;
	size_t size;
};
