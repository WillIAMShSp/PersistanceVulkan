/*****************************************************************//**
 * @file   DescriptorSet.h
 * @brief  Function declarations for descriptor set allocation and write descriptor set creation
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/

#pragma once
#include "../PersistanceLib.h"
#include <vector>
#include <array>
#include <memory>
#include "Utility.h"


namespace PersistanceBackend
{
	std::vector<VkDescriptorSet> allocateDescriptorSetArray(VkDescriptorPool& descriptorPool, uint32_t descriptorSetCount, VkDescriptorSetLayout& layout);
	VkWriteDescriptorSet createWriteDescriptorSet(uint32_t descriptorCount, VkDescriptorType descriptorType, uint32_t binding, VkDescriptorBufferInfo* bufferInfo, VkDescriptorImageInfo* imageInfo, uint32_t dstArrayElement);
	std::array<VkWriteDescriptorSet, PersistanceLib::MAXFRAMESINFLIGHT> createWriteDescriptorSetsPerFrame(const uint32_t descriptorCount, const VkDescriptorType descriptorType, uint32_t binding, PersistanceUtils::ArrayView<VkDescriptorBufferInfo> bufferInfo = {nullptr, 0}, PersistanceUtils::ArrayView<VkDescriptorImageInfo> imageInfo = {nullptr, 0}, uint32_t dstArrayElement = 0);


	VkDescriptorBufferInfo createDescriptorBufferInfo(const VkBuffer& buffer, uint32_t offset, size_t range);
	std::vector<VkDescriptorBufferInfo> createDescriptorBufferInfoPerFrame(const VkBuffer* buffers, size_t bufferCount, uint32_t offset, size_t range);
	
	VkDescriptorImageInfo createDescriptorImageInfo(const VkImageLayout imageLayout, VkImageView& imageView, VkSampler& sampler);
	std::vector<VkDescriptorImageInfo> createDescriptorImageInfoPerFrameWithArray(const VkImageLayout imageLayout, std::vector<VkImageView>& imageViews, VkSampler& sampler);
	
	void updateDescriptorSet(VkDescriptorSet& descriptorSet, VkWriteDescriptorSet* writeDescriptorSets, const uint32_t writeDescriptorCount);
	void updateDescriptorSetsPerFrame(std::vector<VkDescriptorSet>& descriptorSet, VkWriteDescriptorSet* writeDescriptorSets, const uint32_t writeDescriptorCount);
	void updateDescriptorSetPerFrameWithArray(std::vector<VkDescriptorSet>& descriptorSet, PersistanceUtils::ArrayView<VkWriteDescriptorSet> writeDescriptorSets = {nullptr, 0});
	
}
