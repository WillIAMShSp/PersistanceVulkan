#pragma once
#include "../PersistanceLib.h"
#include <vector>


typedef uint32_t UniformBufferHandle;

struct UniformBuffer
{
	UniformBuffer()
	{
		buffers.resize(PersistanceLib::MAXFRAMESINFLIGHT);
		memorymaps.resize(PersistanceLib::MAXFRAMESINFLIGHT);
		allocations.resize(PersistanceLib::MAXFRAMESINFLIGHT);
		size = 0;
	}

	std::vector<VkBuffer> buffers;
	std::vector<VmaAllocation> allocations;
	std::vector<void*> memorymaps;
	size_t size;

};