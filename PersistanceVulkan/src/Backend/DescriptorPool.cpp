/*****************************************************************//**
 * @file   DescriptorPool.cpp
 * @brief  The function definitions for descriptor pool creation and destruction
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/
#include "DescriptorPool.h"
#include "../Core/PersistanceVkCore.h"


/**
 * @brief Create a descriptor pool size.
 * 
 * @param type The type of descriptor the size is for.
 * @return a descriptor pool size
 */
VkDescriptorPoolSize PersistanceBackend::createDescriptorPoolSize(VkDescriptorType type)
{
	VkDescriptorPoolSize size;
	
	size.type = type;
	size.descriptorCount = PersistanceLib::MAXFRAMESINFLIGHT;
	return size;


}

/**
 * @brief creates a descriptor pool.
 * 
 * @param list A list of descriptor pool sizes.
 * @return The resulting descriptor pool.
 */
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

	if (vkCreateDescriptorPool(core.getDevice(), &info, nullptr, &pool) != VK_SUCCESS) 
	{
		BREAK(0);
	}

	return pool;


}

void PersistanceBackend::cleanUpDescriptorPools(VkDescriptorPool* descriptorPools, const uint32_t descriptorPoolCount)
{

	for (int i = 0; i < descriptorPoolCount; i++) 
	{
		vkDestroyDescriptorPool(core.getDevice(), descriptorPools[i], nullptr);
	}

}
