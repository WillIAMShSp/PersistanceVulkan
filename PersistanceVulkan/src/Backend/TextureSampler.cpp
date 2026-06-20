#include "TextureSampler.h"
#include "../Core/PersistanceVkCore.h"

VkSampler PersistanceBackend::createTextureSampler(VkFilter magfilter, VkFilter minfilter, VkSamplerAddressMode addressmodeU, VkSamplerAddressMode addressmodeV, VkSamplerAddressMode addressmodeW, VkBorderColor bordercolor, VkSamplerMipmapMode mipmapmode, float miplodbias, float minlod, float maxlod, bool anisotropy)
{

	VkSampler sampler;

	VkPhysicalDeviceProperties properties{};

	vkGetPhysicalDeviceProperties(core.m_physicalDevice, &properties);



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

	if (vkCreateSampler(core.m_device, &samplerinfo, nullptr, &sampler) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create sampler");

	}

	return sampler;
}
