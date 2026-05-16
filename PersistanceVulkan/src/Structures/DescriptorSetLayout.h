#pragma once
#include "PersistanceLib.h"
#include <vector>

typedef uint32_t DescriptorSetLayoutHandle;

struct DescriptorSetLayout
{

	VkDescriptorSetLayout layout;
	std::vector<VkDescriptorSetLayoutBinding> bindings;


};