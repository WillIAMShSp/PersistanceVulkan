#pragma once
#include "PersistanceLib.h"
#include <vector>

typedef uint32_t TextureHandle;
typedef uint32_t TextureSamplerHandle;

struct Texture
{
	VkImage image;
	VkImageView imageview;
	VmaAllocation allocation;
	int width;
	int height;
};

