#pragma once
#include "PersistanceLib.h"

namespace PersistanceBackend {



	VkDescriptorSetLayoutBinding createDescriptorSetLayoutBinding(uint32_t bindingidx, VkDescriptorType descriptortype, VkShaderStageFlagBits shaderstage);

	VkDescriptorSetLayout createDescriptorSetLayout(const VkDescriptorSetLayoutBinding* bindings, const uint32_t bindingCount);


}
