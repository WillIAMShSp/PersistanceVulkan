#pragma once
#include "PersistanceLib.h"
#include <vector>

typedef uint32_t FramebufferHandle;

struct Framebuffer {
	Framebuffer() = default;

	std::vector<VkFramebuffer> framebuffers;
	std::vector<VkImageView> imageviews;
	std::vector<VkImage> images;
	std::vector<VmaAllocation> allocations;

};
