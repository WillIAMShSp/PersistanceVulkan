#pragma once
#include "../PersistanceLib.h"
#include <vector>
#include <array>

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

template <size_t N>
struct MultiTexture
{
	std::array<VkImage, N> images;
	std::array<VkImageView, N> imageViews;
	std::array<VmaAllocation, N> allocations;
	size_t size = N;
	int width;
	int height;
};

