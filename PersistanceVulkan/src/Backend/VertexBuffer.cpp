/*****************************************************************//**
 * @file   VertexBuffer.cpp
 * @brief  Function definitions for vertex buffer creation and destruction.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/

#include "VertexBuffer.h"
#include "../Core/CoreUtils.h"

/**
 * @brief Creates a vertex buffer.
 * 
 * @param buffer The created vertex buffer's data.
 * @param elementSize The size of an element in the data.
 * @param elementCount The amount of elements in the data.
 * @return The created vertex buffer.
 */
Buffer PersistanceBackend::createVertexBuffer(const void* buffer, const size_t elementSize, const uint32_t elementCount)
{
	Buffer vertexBuffer;

	Buffer stagingBuffer;

	size_t bufferSize = elementSize * elementCount;

	PersistanceUtils::createBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer.buffer, stagingBuffer.allocation, VK_SHARING_MODE_CONCURRENT);

	void* data;

	vmaMapMemory(core.m_vmaAllocator, stagingBuffer.allocation, &data);
	memcpy(data, buffer, bufferSize);
	vmaUnmapMemory(core.m_vmaAllocator, stagingBuffer.allocation);

	PersistanceUtils::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer.buffer, vertexBuffer.allocation, VK_SHARING_MODE_CONCURRENT);

	PersistanceUtils::copyBuffer(stagingBuffer.buffer, vertexBuffer.buffer, bufferSize, core.m_transferCommandPool, core.m_transferQueue);

	vmaDestroyBuffer(core.m_vmaAllocator, stagingBuffer.buffer, stagingBuffer.allocation);

	vertexBuffer.size = bufferSize;
	vertexBuffer.elementSize = elementSize;

	return vertexBuffer;

}

/**
 * @brief Destroys all the vertex buffers specified..
 * 
 * @param buffers Vertex buffers specified.
 * @param bufferCount The amount of vertex buffers.
 */
void PersistanceBackend::cleanUpVertexBuffers(Buffer* buffers, const uint32_t bufferCount)
{
	for (uint32_t i = 0; i < bufferCount; i++) 
	{
		vmaDestroyBuffer(core.m_vmaAllocator, buffers[i].buffer, buffers[i].allocation);
	
	}
}
