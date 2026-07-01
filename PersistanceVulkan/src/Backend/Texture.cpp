/*****************************************************************//**
 * @file   Texture.cpp
 * @brief  Function definitions for texture creation and destruction.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/

#include "Texture.h"
#include "../Core/CoreUtils.h"

/**
 * @brief Creates a texture with the specified image path.
 * Breaks if texture creation unsuccessful.
 * 
 * @param imagePath The path of the texture image.
 * @return A texture with the image found in the path.
 */
Texture PersistanceBackend::createTexture(const char* imagePath)
{
	Texture finalTexture;


	int width, height, bpp;

	stbi_uc* pixels = stbi_load(imagePath, &width, &height, &bpp, STBI_rgb_alpha);
	VkDeviceSize buffersize = width * height * 4;

	if (!pixels)
	{
		throw std::runtime_error("Didnt find the texture!");
	}

	VkBuffer stagingbuffer;
	VmaAllocation stagingalloc;

	PersistanceUtils::createBuffer(buffersize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingbuffer, stagingalloc, VK_SHARING_MODE_CONCURRENT);


	void* data;


	vmaMapMemory(core.m_vmaAllocator, stagingalloc, &data);
	memcpy(data, pixels, static_cast<uint32_t>(buffersize));
	vmaUnmapMemory(core.m_vmaAllocator, stagingalloc);

	stbi_image_free(pixels);

	PersistanceUtils::createImage(width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		finalTexture.image, finalTexture.allocation, VK_SHARING_MODE_CONCURRENT, VK_IMAGE_LAYOUT_UNDEFINED);


	PersistanceUtils::transitionImageLayout(finalTexture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, core.m_transferCommandPool, core.m_transferQueue);

	PersistanceUtils::copyBufferToImage(stagingbuffer, finalTexture.image, static_cast<uint32_t>(width), static_cast<uint32_t>(height), core.m_transferCommandPool, core.m_transferQueue);

	PersistanceUtils::transitionImageLayout(finalTexture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, core.m_graphicsCommandPool, core.m_graphicsQueue);

	vmaDestroyBuffer(core.m_vmaAllocator, stagingbuffer, stagingalloc);

	
	finalTexture.imageview = PersistanceUtils::createImageView(finalTexture.image, VK_FORMAT_R8G8B8A8_SRGB);

	return finalTexture;

}

/**
 * @brief Cleans up specified textures, including their images and image views.
 * 
 * @param textures Specified textures in an array.
 * @param textureCount The amount of textures in the texture array.
 */
void PersistanceBackend::cleanUpTextures(Texture* textures, const uint32_t textureCount)
{
	for (uint32_t i = 0; i < textureCount; i++) 
	{
		vmaDestroyImage(core.m_vmaAllocator, textures[i].image, textures[i].allocation);
		vkDestroyImageView(core.m_device, textures[i].imageview, nullptr);
	}
}


