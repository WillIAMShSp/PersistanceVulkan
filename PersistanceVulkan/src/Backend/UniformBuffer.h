/*****************************************************************//**
 * @file   UniformBuffer.h
 * @brief  Function declaration for uniform buffer creation, update, and destruction.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/

#pragma once
#include "PersistanceLib.h"
#include "../Structures/UniformBuffer.h"

namespace PersistanceBackend 
{
	UniformBuffer createUniformBuffer(size_t size);

	void updateUniformBuffers(UniformBuffer& buffer, const void* data, const size_t dataSize);

	void cleanUpUniformBuffers(UniformBuffer* buffers, const uint32_t bufferCount);
}
