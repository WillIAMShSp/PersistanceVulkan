/*****************************************************************//**
 * @file   DescriptorSetLayout.h
 * @brief  Function declarations for descriptor set layout creation and destruction.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/

#pragma once
#include "PersistanceLib.h"

namespace PersistanceBackend {



	VkDescriptorSetLayoutBinding createDescriptorSetLayoutBinding(uint32_t bindingidx, VkDescriptorType descriptortype, VkShaderStageFlagBits shaderstage);

	VkDescriptorSetLayout createDescriptorSetLayout(const VkDescriptorSetLayoutBinding* bindings, const uint32_t bindingCount);

	void cleanUpDescriptorSetLayouts(VkDescriptorSetLayout* descriptorSetLayouts, const uint32_t descriptorSetLayoutCount);


}
