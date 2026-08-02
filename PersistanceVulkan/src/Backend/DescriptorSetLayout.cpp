/*****************************************************************//**
 * @file   DescriptorSetLayout.cpp
 * @brief  Function definitions for descriptor set layout creation and destruction.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/
#include "DescriptorSetLayout.h"
#include "../Core/PersistanceVkCore.h"


/**
 * @brief Creates a binding point for a descriptor set.
 * 
 * @param bindingidx The binding index that must be referenced in the shader.
 * @param descriptortype The type of descriptor for this binding.
 * @param shaderstage The stage in the shader pipeline where this binding will be used.
 * @return A binding point for a descriptor set layout.
 */
VkDescriptorSetLayoutBinding PersistanceBackend::createDescriptorSetLayoutBinding(uint32_t bindingidx, VkDescriptorType descriptortype, VkShaderStageFlagBits shaderstage)
{
	VkDescriptorSetLayoutBinding binding{};
	
	binding.binding = bindingidx;
	binding.descriptorCount = 1;
	binding.descriptorType = descriptortype;
	binding.pImmutableSamplers = 0;
	binding.stageFlags = shaderstage;

	return binding;


}

/**
 * @brief Creates a descriptor set layout with the provided bindings.
 * 
 * @param bindings The provided bindings.
 * @param bindingCount The amount of provided bindings.
 * @return A descriptor set layout.
 */
VkDescriptorSetLayout PersistanceBackend::createDescriptorSetLayout(const VkDescriptorSetLayoutBinding* bindings, const uint32_t bindingCount)
{
	VkDescriptorSetLayout layout;
	
	VkDescriptorSetLayoutCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	info.pBindings = bindings;
	info.bindingCount = bindingCount;
	info.flags = 0;

	if (vkCreateDescriptorSetLayout(core.getDevice(), &info, nullptr, &layout) != VK_SUCCESS) {
		BREAK(0);

	};

	return layout;

}


/**
 * @brief Destroys all descriptor set layouts specified.
 * 
 * @param descriptorSetLayouts Layouts specified.
 * @param descriptorSetLayoutCount Amount of layouts specified.
 */
void PersistanceBackend::cleanUpDescriptorSetLayouts(VkDescriptorSetLayout* descriptorSetLayouts, const uint32_t descriptorSetLayoutCount)
{
	for (int i = 0; i < descriptorSetLayoutCount; i++) 
	{
		vkDestroyDescriptorSetLayout(core.getDevice(), descriptorSetLayouts[i], nullptr);

	}

}
