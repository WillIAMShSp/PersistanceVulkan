#include "DescriptorSetLayout.h"
#include "../Core/PersistanceVkCore.h"

VkDescriptorSetLayoutBinding DescriptorSetLayoutFunc::createDescriptorSetLayoutBinding(uint32_t bindingidx, VkDescriptorType descriptortype, VkShaderStageFlagBits shaderstage)
{
	VkDescriptorSetLayoutBinding binding{};
	
	binding.binding = bindingidx;
	binding.descriptorCount = 1;
	binding.pImmutableSamplers = 0;
	binding.stageFlags = shaderstage;

	return binding;


}

VkDescriptorSetLayout DescriptorSetLayoutFunc::createDescriptorSetLayout(const VkDescriptorSetLayoutBinding* bindings, const uint32_t bindingCount)
{
	VkDescriptorSetLayout layout;
	
	VkDescriptorSetLayoutCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	info.pBindings = bindings;
	info.bindingCount = bindingCount;

	if (vkCreateDescriptorSetLayout(core.m_device, &info, nullptr, &layout) != VK_SUCCESS) {
		BREAK(0);

	};

	return layout;

}
