#include "CommandBuffer.h"
#include "../Core/PersistanceVkCore.h"


VkCommandBuffer PersistanceBackend::allocateCommandBuffer(VkCommandPool commandPool, VkCommandBufferLevel commandBufferLevel)
{
	VkCommandBuffer commandBuffer;

	VkCommandBufferAllocateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.commandBufferCount = 1;
	info.level = commandBufferLevel;
	info.commandPool = commandPool;


	if (vkAllocateCommandBuffers(core.m_device, &info, &commandBuffer) != VK_SUCCESS) 
	{
		BREAK(0);
	}

	return commandBuffer;

}

/**
 * Begins a command buffer.
 * 
 * @param commandBuffer the command buffer to initialize.
 * @param usageFlags options for the command buffer utilization.
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

/// <summary>
/// Ends a command Buffer
/// </summary>
/// <param name="commandBuffer"> command buffer to end</param>
void PersistanceBackend::endCommandBuffer(VkCommandBuffer commandBuffer)
{
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		BREAK(0);
	};
}

void PersistanceBackend::resetCommandBuffer(VkCommandBuffer commandBuffer)
{
	vkResetCommandBuffer(commandBuffer, 0);

}
