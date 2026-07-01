/*****************************************************************//**
 * @file   IndexBuffer.cpp
 * @brief  Function definition for index buffer creation and destruction.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/
#include "IndexBuffer.h"
#include "../Core/CoreUtils.h"

/**
 * @brief Creates an index buffer.
 * 
 * @param buffer The index buffer data.
 * @param elementSize The size of the index buffer element. i.e. uint32_t.
 * @param elementCount The amount of indices provided.
 * @return An index buffer with the data specified.
 */
Buffer PersistanceBackend::createIndexBuffer(const void* buffer, const size_t elementSize, const uint32_t elementCount)
{
	Buffer indexBuffer;

	Buffer stagingBuffer;

	size_t bufferSize = elementSize * elementCount;

	PersistanceUtils::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer.buffer, stagingBuffer.allocation, VK_SHARING_MODE_CONCURRENT);

	void* data;

	vmaMapMemory(core.m_vmaAllocator, stagingBuffer.allocation, &data);
	memcpy(data, buffer, bufferSize);
	vmaUnmapMemory(core.m_vmaAllocator, stagingBuffer.allocation);

	PersistanceUtils::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer.buffer, indexBuffer.allocation, VK_SHARING_MODE_CONCURRENT);

	PersistanceUtils::copyBuffer(stagingBuffer.buffer, indexBuffer.buffer, bufferSize, core.m_transferCommandPool, core.m_transferQueue);

	indexBuffer.size = bufferSize;

	vmaDestroyBuffer(core.m_vmaAllocator, stagingBuffer.buffer, stagingBuffer.allocation);

	return indexBuffer;

}

/**
 * @brief Destroys provided index buffers.
 * 
 * @param buffers The provided index buffers.
 * @param bufferCount The amount of index buffers to destroy.
 */
void PersistanceBackend::cleanUpIndexBuffers(Buffer* buffers, const uint32_t bufferCount)
{
	for (uint32_t i = 0; i < bufferCount; i++) 
	{
		vmaDestroyBuffer(core.m_vmaAllocator, buffers[i].buffer, buffers[i].allocation);
	}

}
