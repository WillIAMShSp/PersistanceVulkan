/*****************************************************************//**
 * @file   DescriptorSet.h
 * @brief  Function declarations for descriptor set allocation and write descriptor set creation
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/

#pragma once
#include "../PersistanceLib.h"
#include <vector>

namespace PersistanceBackend
{
	std::vector<VkDescriptorSet> allocateDescriptorSet(VkDescriptorPool& descriptorPool, uint32_t descriptorSetCount, VkDescriptorSetLayout& layout);
	VkWriteDescriptorSet createWriteDescriptorSet(uint32_t descriptorCount, VkDescriptorType descriptorType, uint32_t binding, VkDescriptorBufferInfo* bufferInfo, VkDescriptorImageInfo* imageInfo, uint32_t dstArrayElement);
	std::vector<VkDescriptorBufferInfo> createDescriptorBufferInfo(const VkBuffer* buffers, const uint32_t bufferCount, uint32_t offset, size_t range);
	std::vector<VkDescriptorImageInfo> createDescriptorImageInfo(const VkImageLayout imageLayout, VkImageView& imageView, VkSampler& sampler);
	std::vector<VkDescriptorImageInfo> createDescriptorImageInfoPerFrame(const VkImageLayout imageLayout, std::vector<VkImageView>& imageViews, VkSampler& sampler);
	void updateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSet, VkWriteDescriptorSet* writeDescriptorSets, const uint32_t writeDescriptorCount);
}
