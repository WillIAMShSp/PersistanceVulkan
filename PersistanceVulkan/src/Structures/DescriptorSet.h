#pragma once
#include "PersistanceLib.h"
#include "WriteDesciptorSet.h"
#include <vector>

typedef uint32_t DescriptorSetHandle;

struct DescriptorSet
{
	std::vector<VkDescriptorSet> descriptorsets;
	std::vector<WriteDescriptorSet> writedescriptorsets;

};