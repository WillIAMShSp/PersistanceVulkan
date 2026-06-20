#pragma once
#include "../PersistanceLib.h"
#include "../Structures/Buffer.h"
namespace PersistanceBackend 
{
	Buffer createIndexBuffer(const void* buffer, const size_t elementSize, const uint32_t elementCount);
}
