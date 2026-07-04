/*****************************************************************//**
 * @file   DescriptorPool.h
 * @brief  Functions for descriptor pool creation and destruction.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/
#pragma once
#include "../PersistanceLib.h"
#include "../Structures/DescriptorPool.h"

namespace PersistanceBackend
{

	VkDescriptorPoolSize createDescriptorPoolSize(VkDescriptorType type);

	VkDescriptorPool createDescriptorPool(DescriptorPoolSizeList& list);

	void cleanUpDescriptorPools(VkDescriptorPool* descriptorPools, const uint32_t descriptorPoolCount);


}

