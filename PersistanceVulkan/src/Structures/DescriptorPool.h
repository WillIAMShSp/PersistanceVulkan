#pragma once
#include "PersistanceLib.h"
#include <vector>

struct DescriptorPool
{
	std::vector<VkDescriptorPoolSize> poolsizes;
	VkDescriptorPool pool;
};