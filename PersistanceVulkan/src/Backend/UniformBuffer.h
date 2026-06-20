#pragma once
#include "PersistanceLib.h"
#include "../Structures/UniformBuffer.h"

namespace PersistanceBackend 
{
	UniformBuffer createUniformBuffer(size_t size);

	void updateUniformBuffers(UniformBuffer& buffer, const void* data, const size_t bufferSize);

}
