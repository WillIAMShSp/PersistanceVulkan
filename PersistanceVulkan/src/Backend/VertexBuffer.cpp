#include "VertexBuffer.h"
#include "../Core/CoreUtils.h"

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

	return vertexBuffer;

}
