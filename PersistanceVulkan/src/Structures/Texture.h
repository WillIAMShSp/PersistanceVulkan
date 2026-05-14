#pragma once
#include "PersistanceLib.h"
#include <vector>

struct Texture
{
	VkImage image;
	VkDeviceMemory memory;
	VkImageView imageview;
	VmaAllocation allocation;
	int width;
	int height;
};

