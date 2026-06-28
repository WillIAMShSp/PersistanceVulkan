#include "DescriptorSet.h"
#include "../Core/PersistanceVkCore.h"

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


