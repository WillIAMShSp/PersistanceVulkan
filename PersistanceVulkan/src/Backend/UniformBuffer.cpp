#include "UniformBuffer.h"
#include "../Core/CoreUtils.h"

UniformBuffer PersistanceBackend::createUniformBuffer(size_t size)
{
	UniformBuffer uniformBuffer;
	
	for (uint32_t i = 0; i < PersistanceLib::MAXFRAMESINFLIGHT; i++) 
	{
		PersistanceUtils::createBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, uniformBuffer.buffers[i], uniformBuffer.allocations[i], VK_SHARING_MODE_CONCURRENT);
		vmaMapMemory(core.m_vmaAllocator, uniformBuffer.allocations[i], &uniformBuffer.memorymaps[i]);
	}

	return uniformBuffer;

}

void PersistanceBackend::updateUniformBuffers(UniformBuffer& buffer, const void* data, const size_t bufferSize)
{
	memcpy(buffer.memorymaps[core.m_currentFrame], data, bufferSize);
}


