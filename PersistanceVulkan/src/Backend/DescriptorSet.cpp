/*****************************************************************//**
 * @file   DescriptorSet.cpp
 * @brief  Function definitions for descriptor set allocation and write descriptor set creation
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/

#include "DescriptorSet.h"
#include "../Core/PersistanceVkCore.h"

/**
 * @brief Allocates a vector of descriptor sets of size equal to the descriptorSetCount parameter from a descriptor pool.
 * 
 * @param descriptorPool The descriptor pool used for allocation.
 * @param descriptorSetCount The amount of descriptor sets created.
 * @param layout The descriptor set layout for these descriptorSets.
 * @return A vector of descriptor sets.
 */
std::vector<VkDescriptorSet> PersistanceBackend::allocateDescriptorSet(VkDescriptorPool& descriptorPool, uint32_t descriptorSetCount, VkDescriptorSetLayout& layout)
{
	std::vector<VkDescriptorSet> descriptorSet;
	descriptorSet.resize(PersistanceLib::MAXFRAMESINFLIGHT);


	std::vector<VkDescriptorSetLayout> layouts(static_cast<uint32_t>(PersistanceLib::MAXFRAMESINFLIGHT), layout);

	VkDescriptorSetAllocateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	info.descriptorPool = descriptorPool;
	info.pSetLayouts = layouts.data();
	info.descriptorSetCount = descriptorSetCount;
	info.pNext = nullptr;

	if (vkAllocateDescriptorSets(core.m_device, &info, descriptorSet.data()) != VK_SUCCESS) {
		BREAK(0);
	}
	return descriptorSet;
}

/**
 * @brief Creates a write descriptor set.
 * 
 * @param descriptorCount The amount of descriptors in the write descriptor set.
 * @param descriptorType The type of the descriptor.
 * @param binding The descriptor binding.
 * @param bufferInfo A provided descriptor buffer informaiton object (i.e. A uniform buffer).
 * @param imageInfo A provided descriptor image information object (i.e. information about an image view and its sampler for textures).
 * @param dstArrayElement The initial array element.
 * @return The configured write descriptor set.
 */
VkWriteDescriptorSet PersistanceBackend::createWriteDescriptorSet(uint32_t descriptorCount, VkDescriptorType descriptorType, uint32_t binding, VkDescriptorBufferInfo* bufferInfo, VkDescriptorImageInfo* imageInfo, uint32_t dstArrayElement)
{
	VkWriteDescriptorSet writeDescriptorSet{};

	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.dstBinding = binding;
	writeDescriptorSet.descriptorType = descriptorType;
	writeDescriptorSet.pBufferInfo = bufferInfo;
	writeDescriptorSet.pImageInfo = imageInfo;
	writeDescriptorSet.descriptorCount = descriptorCount;
	writeDescriptorSet.dstArrayElement = dstArrayElement;

	return writeDescriptorSet;
}

/**
 * @brief Creates a vector of descriptor buffer info objects utilized in a write descriptor set object.
 * 
 * @param buffers An array of buffers included in the buffer information.
 * @param bufferCount The size of the array of buffers.
 * @param offset An offset since the beginning of the array from the first element desired in the buffers.
 * @param range Is the size in bytes that is used for this descriptor update, or VK_WHOLE_SIZE to use the range from offset to the end of the buffer.
 * @return A descriptor buffer info vector.
 */
std::vector<VkDescriptorBufferInfo> PersistanceBackend::createDescriptorBufferInfo(const VkBuffer* buffers, const uint32_t bufferCount, uint32_t offset, size_t range)
{
	std::vector<VkDescriptorBufferInfo> bufferInfos;
	bufferInfos.resize(PersistanceLib::MAXFRAMESINFLIGHT);

	for (int i = 0; i < PersistanceLib::MAXFRAMESINFLIGHT; i++)
	{
		bufferInfos[i].buffer = buffers[i];
		bufferInfos[i].offset = offset;
		bufferInfos[i].range = range;
	}


	return bufferInfos;

}

/**
 * @brief Creates a vector of descriptor image info objects with a provided image and sampler utilized in a write descriptor set object.
 * 
 * @param imageLayout The layout of the provided image in the image view.
 * @param imageView The imageview object of the provided image.
 * @param sampler The provided sampler for the image view.
 * @return A descriptor image info vector.
 */
std::vector<VkDescriptorImageInfo> PersistanceBackend::createDescriptorImageInfo(const VkImageLayout imageLayout, VkImageView& imageView, VkSampler& sampler)
{
	std::vector<VkDescriptorImageInfo> imageInfos;
	imageInfos.resize(PersistanceLib::MAXFRAMESINFLIGHT);

	for (int i = 0; i < PersistanceLib::MAXFRAMESINFLIGHT; i++) 
	{
		imageInfos[i].imageLayout = imageLayout;
		imageInfos[i].imageView = imageView;
		imageInfos[i].sampler = sampler;
		
	}

	return imageInfos;
}


/**
 * @brief Creates a vector of descriptor image info objects with a provided image per frame and a sampler utilized in a write descriptor set object.
 * If the size of the imageViews vector is not equal to the amount of frames in flight, the function will break.
 * 
 * @param imageLayout The layout of the provided images in the image view.
 * @param imageViews The imageview objects of the provided image.
 * @param sampler
 * @return 
 */
std::vector<VkDescriptorImageInfo> PersistanceBackend::createDescriptorImageInfoPerFrame(const VkImageLayout imageLayout, std::vector<VkImageView>& imageViews, VkSampler& sampler)
{
	if (imageViews.size() != PersistanceLib::MAXFRAMESINFLIGHT) 
	{
		std::cout << "ERROR: The imageViews Vector in createDescriptorImageInfoPerFrame() function does not have the right amount of VkImageView objects.";
		BREAK(0);
	}

	std::vector<VkDescriptorImageInfo> imageInfos;
	imageInfos.resize(PersistanceLib::MAXFRAMESINFLIGHT);

	for (int i = 0; i < PersistanceLib::MAXFRAMESINFLIGHT; i++)
	{
		imageInfos[i].imageLayout = imageLayout;
		imageInfos[i].imageView = imageViews[i];
		imageInfos[i].sampler = sampler;

	}

	return imageInfos;
}

/**
 * @brief Updates the descriptor sets with the write descriptor set objects.
 * 
 * @param descriptorSet The updated descriptor sets.
 * @param writeDescriptorSets The write descriptor sets used.
 * @param writeDescriptorCount The amount of write descriptor sets.
 */
void PersistanceBackend::updateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSet, VkWriteDescriptorSet* writeDescriptorSets, const uint32_t writeDescriptorCount)
{
	for (int i = 0; i < PersistanceLib::MAXFRAMESINFLIGHT; i++) 
	{
		
		for (int j = 0; j < writeDescriptorCount; j++) 
		{
			writeDescriptorSets[j].dstSet = descriptorSet[i];
		}
		
		vkUpdateDescriptorSets(core.m_device, static_cast<uint32_t>(writeDescriptorCount), writeDescriptorSets, 0, nullptr);
		
	}

}




