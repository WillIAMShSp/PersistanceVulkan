#pragma once
#include "PersistanceLib.h"
#include <vector>
struct WriteDescriptorSet
{
	uint32_t arrayelement;
	uint32_t descriptorcount;
	uint32_t bindingidx;
	VkDescriptorType descriptorType;
	std::vector<DescriptorBufferInfo> bufferinfo;
	std::vector<DescriptorImageInfo> imageinfo;
};


