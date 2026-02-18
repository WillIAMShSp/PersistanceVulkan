#pragma once
#include "PersistanceLib.h"
#include <vector>

typedef uint32_t descriptorSetLayoutHandle;

struct DescriptorSetLayout
{

	VkDescriptorSetLayout layout;
	std::vector<VkDescriptorSetLayoutBinding> bindings;


};