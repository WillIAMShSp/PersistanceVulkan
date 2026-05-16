#pragma once
#include "PersistanceLib.h"
#include <vector>

typedef uint32_t DescriptorPoolHandle;

struct DescriptorPool
{
	std::vector<VkDescriptorPoolSize> poolsizes;
	VkDescriptorPool pool;
};