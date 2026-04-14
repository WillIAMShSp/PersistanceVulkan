#pragma once
#include "PersistanceLib.h"
#include "WriteDesciptorSet.h"
#include <vector>

struct DescriptorSet
{
	std::vector<VkDescriptorSet> descriptorsets;
	std::vector<WriteDescriptorSet> writedescriptorsets;

};