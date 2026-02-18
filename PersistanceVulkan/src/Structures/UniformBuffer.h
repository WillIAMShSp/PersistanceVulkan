#pragma once
#include "PersistanceLib.h"
#include <vector>


struct UniformBuffer
{
	UniformBuffer()
	{
		buffers.resize(PersistanceLib::MAXFRAMESINFLIGHT);
		memory.resize(PersistanceLib::MAXFRAMESINFLIGHT);
		memorymaps.resize(PersistanceLib::MAXFRAMESINFLIGHT);
		size = 0;
	}

	std::vector<VkBuffer> buffers;
	std::vector<VkDeviceMemory> memory;
	std::vector<void*> memorymaps;
	size_t size;

};