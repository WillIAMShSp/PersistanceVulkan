/*****************************************************************//**
 * @file   TextureSampler.h
 * @brief  Function declaration for texture sampler creation and destruction.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/

#pragma once
#include "PersistanceLib.h"

namespace PersistanceBackend
{
	VkSampler createTextureSampler(VkFilter magfilter = VK_FILTER_LINEAR,
		VkFilter minfilter = VK_FILTER_LINEAR,
		VkSamplerAddressMode addressmodeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		VkSamplerAddressMode addressmodeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		VkSamplerAddressMode addressmodeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		VkBorderColor bordercolor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
		VkSamplerMipmapMode mipmapmode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		float miplodbias = 0.0f,
		float minlod = 0.0f,
		float maxlod = 0.0f,
		bool anisotropy = false);

	void cleanUpTextureSamplers(VkSampler* samplers, const uint32_t samplerCount);
	


}

