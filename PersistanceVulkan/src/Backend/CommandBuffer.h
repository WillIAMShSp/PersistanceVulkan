/*****************************************************************//**
 * @file   CommandBuffer.h
 * @brief  Function definitions for command buffer creation, initialization, finalization and reset.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/
#pragma once
#include "../PersistanceLib.h"

namespace PersistanceBackend 
{
	VkCommandBuffer allocateCommandBuffer(VkCommandPool commandPool, VkCommandBufferLevel commandBufferLevel);
	void beginCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferUsageFlags usageFlags);
	void endCommandBuffer(VkCommandBuffer commandBuffer);
	void resetCommandBuffer(VkCommandBuffer commandBuffer);
}
