/*****************************************************************//**
 * @file   UniformBuffer.cpp
 * @brief  Function definitions for uniform buffer creation, update, and destruction.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/

#include "UniformBuffer.h"
#include "../Core/CoreUtils.h"

/**
 * @brief Creates a uniform buffer with the specified size.
 * Breaks if uniform buffer creation unsuccessful.
 * 
 * @param size Specified size;
 * @return The created uniform buffer
 */
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

/**
 * @brief Updates a uniform buffer with the provided data.
 * 
 * @param buffer The updated uniform buffer.
 * @param data The provided data.
 * @param dataSize The size of the provided data.
 */
void PersistanceBackend::updateUniformBuffers(UniformBuffer& buffer, const void* data, const size_t dataSize)
{
	memcpy(buffer.memorymaps[core.m_currentFrame], data, dataSize);
}


/**
 * @brief Destroys specified uniform buffers including their images and image views.
 * 
 * @param buffers Specified uniform buffers.
 * @param bufferCount The amount of uniform buffers.
 */
void PersistanceBackend::cleanUpUniformBuffers(UniformBuffer* buffers, const uint32_t bufferCount)
{
	for (uint32_t i = 0; i < bufferCount; i++) 
	{
		for (uint32_t b = 0; b < buffers[i].buffers.size(); b++) 
		{
			vmaUnmapMemory(core.m_vmaAllocator, buffers[i].allocations[b]);
			vmaDestroyBuffer(core.m_vmaAllocator, buffers[i].buffers[b], buffers[i].allocations[b]);
		
		}
		
	}

}


