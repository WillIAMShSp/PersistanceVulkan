/*****************************************************************//**
 * @file   IndexBuffer.h
 * @brief  Function declaration for index buffer creation and destruction.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/

#pragma once
#include "../PersistanceLib.h"
#include "../Structures/Buffer.h"
namespace PersistanceBackend 
{
	Buffer createIndexBuffer(const void* buffer, const size_t elementSize, const uint32_t elementCount);
	void cleanUpIndexBuffers(Buffer* buffers, const uint32_t bufferCount);
}
