/*****************************************************************//**
 * @file   CommandBuffer.cpp
 * @brief  Function definitions for command buffer creation, initialization, finalization and reset.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/
#include "CommandBuffer.h"
#include "../Core/PersistanceVkCore.h"


/**
 * @brief Allocates a command buffer within a command pool and returns it.
 * 
 * @param commandPool The command pool from which to allocate the command buffer
 * @param commandBufferLevel The command buffer level i.e Primary or Secondary
 * @return 
 */
VkCommandBuffer PersistanceBackend::allocateCommandBuffer(VkCommandPool commandPool, VkCommandBufferLevel commandBufferLevel)
{
	VkCommandBuffer commandBuffer;

	VkCommandBufferAllocateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.commandBufferCount = 1;
	info.level = commandBufferLevel;
	info.commandPool = commandPool;


	if (vkAllocateCommandBuffers(core.getDevice(), &info, &commandBuffer) != VK_SUCCESS) 
	{
		BREAK(0);
	}
	return commandBuffer;

}

/**
 * @brief Begins a command buffer.
 * 
 * @param commandBuffer The command buffer to initialize.
 * @param usageFlags Flags defining the command buffer usage.
 */
void PersistanceBackend::beginCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferUsageFlags usageFlags)
{
	VkCommandBufferBeginInfo info{};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info.flags = usageFlags;
	info.pInheritanceInfo = nullptr;



	if (vkBeginCommandBuffer(commandBuffer, &info) != VK_SUCCESS) 
	{
		BREAK(0);
	}

}

/**
 * @brief Ends a command buffer.
 * 
 * @param commandBuffer The ended command buffer.
 */
void PersistanceBackend::endCommandBuffer(VkCommandBuffer commandBuffer)
{
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		BREAK(0);
	};
}

/**
 * @brief Resets a command buffer.
 * 
 * @param commandBuffer The reset command buffer.
 */
void PersistanceBackend::resetCommandBuffer(VkCommandBuffer commandBuffer)
{
	vkResetCommandBuffer(commandBuffer, 0);

}
