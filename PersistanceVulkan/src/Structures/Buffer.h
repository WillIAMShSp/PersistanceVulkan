#pragma once
#include "PersistanceLib.h"

struct Buffer {
	Buffer() = default;

	VkBuffer buffer;
	VkDeviceMemory buffermem;


};
