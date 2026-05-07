#pragma once
#include "PersistanceLib.h"
#include <vector>

struct Framebuffer {
	Framebuffer() = default;

	std::vector<VkFramebuffer> framebuffers;
	std::vector<VkImageView> imageviews;
	std::vector<VkImage> images;
	std::vector<VkDeviceMemory> imagememory;


};
