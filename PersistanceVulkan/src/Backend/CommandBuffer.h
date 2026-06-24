#pragma once
#include "../PersistanceLib.h"

namespace PersistanceBackend 
{
	VkCommandBuffer allocateCommandBuffer(VkCommandPool commandPool, VkCommandBufferLevel commandBufferLevel);
	void beginCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferUsageFlags usageFlags);
	void endCommandBuffer(VkCommandBuffer commandBuffer);
	void resetCommandBuffer(VkCommandBuffer commandBuffer);
}
