#pragma once
#include "PersistanceLib.h"
#include <vector>

struct Texture
{
	VkImage image;
	VkDeviceMemory memory;
	VkImageView imageview;
	int width;
	int height;
};

