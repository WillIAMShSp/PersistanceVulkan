#pragma once
#include "PersistanceLib.h"
#include "../Structures/DescriptorPool.h"

namespace PersistanceBackend
{

	VkDescriptorPoolSize createDescriptorPoolSize(VkDescriptorType type);

	VkDescriptorPool createDescriptorPool(DescriptorPoolSizeList& list);




}

