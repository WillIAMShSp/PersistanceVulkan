#pragma once
#include "PersistanceLib.h"
#include <vector>

typedef uint32_t DescriptorPoolHandle;

struct DescriptorPool
{
	std::vector<VkDescriptorPoolSize> poolsizes;
	VkDescriptorPool pool;
};

struct DescriptorPoolSizeList {

	void add(VkDescriptorPoolSize* sizes, uint32_t count) 
	{
		for (int i = 0; i < count; i++) 
		{
			this->sizes.push_back(sizes[i]);
		}
	}


	std::vector<VkDescriptorPoolSize> sizes;

};
