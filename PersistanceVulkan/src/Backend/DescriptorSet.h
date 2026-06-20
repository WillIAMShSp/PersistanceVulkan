#pragma once
#include "../PersistanceLib.h"
#include <vector>

namespace PersistanceBackend 
{
	VkDescriptorSet createDescriptorSet();
	VkWriteDescriptorSet createWriteDescriptorSet();
	std::vector<VkDescriptorBufferInfo> createDescriptorBufferInfo();


}
