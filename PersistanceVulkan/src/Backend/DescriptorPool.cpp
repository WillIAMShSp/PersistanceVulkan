#include "DescriptorPool.h"
#include "../Core/PersistanceVkCore.h"



VkDescriptorPoolSize PersistanceBackend::createDescriptorPoolSize(VkDescriptorType type)
{
	VkDescriptorPoolSize size;

	size.descriptorCount = PersistanceLib::MAXFRAMESINFLIGHT;
	size.type = type;

	return size;


}

VkDescriptorPool PersistanceBackend::createDescriptorPool(DescriptorPoolSizeList& list)
{
	VkDescriptorPool pool;


	VkDescriptorPoolCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	info.flags = 0;
	info.poolSizeCount = static_cast<uint32_t>(list.sizes.size());
	info.pPoolSizes = list.sizes.data();
	info.flags = 0;
	info.maxSets = PersistanceLib::MAXFRAMESINFLIGHT;

	if (vkCreateDescriptorPool(core.m_device, &info, nullptr, &pool) != VK_SUCCESS) 
	{
		BREAK(0);
	}

	return pool;


}
