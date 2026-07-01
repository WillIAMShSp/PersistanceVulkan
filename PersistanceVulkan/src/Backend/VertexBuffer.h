/*****************************************************************//**
 * @file   VertexBuffer.h
 * @brief  Function declaration for vertex buffer creation and deletion.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/

#pragma once
#include "../PersistanceLib.h"
#include "../Structures/Buffer.h"

namespace PersistanceBackend 
{
	Buffer createVertexBuffer(const void* buffer, const size_t elementSize, const uint32_t elementCount);
	
	void cleanUpVertexBuffers(Buffer* buffers, const uint32_t bufferCount);
}
