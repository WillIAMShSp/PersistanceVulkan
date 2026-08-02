/*****************************************************************//**
 * @file   TextureSampler.cpp
 * @brief  Function definition for texture sampler creation and destruction.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/

#include "TextureSampler.h"
#include "../Core/PersistanceVkCore.h"

/**
 * @brief Creates a texture sampler.
 * Breaks if texture sampler creation unsuccessful.
 * 
 * @param magfilter
 * @param minfilter
 * @param addressmodeU
 * @param addressmodeV
 * @param addressmodeW
 * @param bordercolor
 * @param mipmapmode
 * @param miplodbias
 * @param minlod
 * @param maxlod
 * @param anisotropy
 * @return A sampler with the provided settings.
 */
VkSampler PersistanceBackend::createTextureSampler(VkFilter magfilter, VkFilter minfilter, VkSamplerAddressMode addressmodeU, VkSamplerAddressMode addressmodeV, VkSamplerAddressMode addressmodeW, VkBorderColor bordercolor, VkSamplerMipmapMode mipmapmode, float miplodbias, float minlod, float maxlod, bool anisotropy)
{

	VkSampler sampler;

	VkPhysicalDeviceProperties properties{};

	vkGetPhysicalDeviceProperties(core.getPhysicalDevice(), &properties);



	VkSamplerCreateInfo samplerinfo{};
	samplerinfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

	samplerinfo.magFilter = magfilter;
	samplerinfo.minFilter = minfilter;

	samplerinfo.addressModeU = addressmodeU;
	samplerinfo.addressModeV = addressmodeV;
	samplerinfo.addressModeW = addressmodeW;

	samplerinfo.borderColor = bordercolor;

	samplerinfo.mipmapMode = mipmapmode;
	samplerinfo.mipLodBias = miplodbias;
	samplerinfo.minLod = minlod;
	samplerinfo.maxLod = maxlod;

	samplerinfo.anisotropyEnable = anisotropy;
	samplerinfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;



	samplerinfo.unnormalizedCoordinates = VK_FALSE;
	samplerinfo.compareEnable = VK_FALSE;
	samplerinfo.compareOp = VK_COMPARE_OP_ALWAYS;

	if (vkCreateSampler(core.getDevice(), &samplerinfo, nullptr, &sampler) != VK_SUCCESS)
	{
		std::cout << "Failed to create texture sampler \n";
		BREAK(0);

	}

	return sampler;
}

/**
 * @brief Destroys specified texture samplers.
 * 
 * @param samplers Specified samplers.
 * @param samplerCount Amount of samplers.
 */
void PersistanceBackend::cleanUpTextureSamplers(VkSampler* samplers, const uint32_t samplerCount)
{
	for (uint32_t i = 0; i < samplerCount; i++) 
	{
		vkDestroySampler(core.getDevice(), samplers[i], nullptr);
	}
}
