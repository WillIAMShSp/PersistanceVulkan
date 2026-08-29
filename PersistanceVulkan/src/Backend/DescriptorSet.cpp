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
std::vector<VkDescriptorSet> PersistanceBackend::allocateDescriptorSetArray(VkDescriptorPool& descriptorPool, uint32_t descriptorSetCount, VkDescriptorSetLayout& layout)
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

	if (vkAllocateDescriptorSets(core.getDevice(), &info, descriptorSet.data()) != VK_SUCCESS) {
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
 * @brief Creates a write descriptor set per frame.
 * 
 * @param descriptorCount The amount of descriptors in the write descriptor set.
 * @param descriptorType The type of the descriptor.
 * @param binding The descriptor binding.
 * @param bufferInfo A provided descriptor buffer informaiton object (i.e. A uniform buffer).
 * @param imageInfo A provided descriptor image information object (i.e. information about an image view and its sampler for textures).
 * @param dstArrayElement The initial array element.
 * @return The configured write descriptor set.
 */
std::array<VkWriteDescriptorSet, PersistanceLib::MAXFRAMESINFLIGHT> PersistanceBackend::createWriteDescriptorSetsPerFrame(const uint32_t descriptorCount, const VkDescriptorType descriptorType, uint32_t binding, PersistanceUtils::ArrayView<VkDescriptorBufferInfo> bufferInfo, PersistanceUtils::ArrayView<VkDescriptorImageInfo> imageInfo, uint32_t dstArrayElement)
{

	if (descriptorCount == 0) {
		std::cout<< "Error: descriptorCount cannot be 0\n";
		BREAK;
	}

    std::array<VkWriteDescriptorSet, PersistanceLib::MAXFRAMESINFLIGHT> writeDescriptorSets{};
	
	if (bufferInfo.data() != nullptr && !bufferInfo.empty() && (bufferInfo.size() % descriptorCount != 0  || bufferInfo.size() / descriptorCount > PersistanceLib::MAXFRAMESINFLIGHT)) {
		std::cout<< "Error: Invalid number of descriptorCount in relation to bufferInfo structs!\n";
		BREAK;
	}
	if (imageInfo.data() != nullptr && !imageInfo.empty() && (imageInfo.size() % descriptorCount != 0 || imageInfo.size() / descriptorCount > PersistanceLib::MAXFRAMESINFLIGHT)) {
		std::cout<< "Error: Invalid number of descriptorCount in relation to imageInfo structs!\n";
		BREAK;
	}

	uint32_t infoIndex = 0;

	for (uint32_t i = 0; i < PersistanceLib::MAXFRAMESINFLIGHT; i++) {

		writeDescriptorSets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[i].dstBinding = binding;
		writeDescriptorSets[i].descriptorType = descriptorType;
		writeDescriptorSets[i].descriptorCount = descriptorCount;
		writeDescriptorSets[i].dstArrayElement = dstArrayElement;
		
		writeDescriptorSets[i].pBufferInfo = bufferInfo.subView(infoIndex, descriptorCount).data();
		writeDescriptorSets[i].pImageInfo = imageInfo.subView(infoIndex, descriptorCount).data();
		infoIndex += descriptorCount;
	}
	

	return writeDescriptorSets;
}


/**
 * @brief Creates a descriptor buffer info object utilized in a write descriptor set object.
 * 
 * @param buffer A buffer included in the buffer information.
 * @param offset An offset since the beginning of the array from the first element desired in the buffer.
 * @param range Is the size in bytes that is used for this descriptor update, or VK_WHOLE_SIZE to use the range from offset to the end of the buffer.
 * @return A descriptor buffer info struct.
 */
VkDescriptorBufferInfo PersistanceBackend::createDescriptorBufferInfo(const VkBuffer& buffer, uint32_t offset, size_t range)
{
	VkDescriptorBufferInfo bufferInfo;
	
	bufferInfo.buffer = buffer;
	bufferInfo.offset = offset;
	bufferInfo.range = range;

	return bufferInfo;

}

/**
 * @brief Creates a vector of descriptor buffer info objects per frame utilized in a write descriptor set object.
 * 
 * @param bufferArray An array of buffers included in the buffer information.
 * @param bufferCount The size of the array of buffers.
 * @param offset An offset since the beginning of the array from the first element desired in the buffers.
 * @param range Is the size in bytes that is used for this descriptor update, or VK_WHOLE_SIZE to use the range from offset to the end of the buffer.
 * @return A descriptor buffer info vector.
 */
std::vector<VkDescriptorBufferInfo> PersistanceBackend::createDescriptorBufferInfoPerFrame(const VkBuffer* bufferArray, size_t bufferCount, uint32_t offset, size_t range)
{
	if (bufferCount != PersistanceLib::MAXFRAMESINFLIGHT) 
	{
		std::cout<< "Buffers array is of the incorrect size!\nExpected: " + std::to_string(PersistanceLib::MAXFRAMESINFLIGHT) + " Got: "+  std::to_string(bufferCount) + "\n";
		BREAK;
	}

	std::vector<VkDescriptorBufferInfo> bufferInfos;
	bufferInfos.resize(PersistanceLib::MAXFRAMESINFLIGHT);


	for (int i = 0; i < PersistanceLib::MAXFRAMESINFLIGHT; i++)
	{

		
		bufferInfos[i].buffer = bufferArray[i];
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
VkDescriptorImageInfo PersistanceBackend::createDescriptorImageInfo(const VkImageLayout imageLayout, VkImageView& imageView, VkSampler& sampler)
{
	VkDescriptorImageInfo imageInfos;
	
	
	imageInfos.imageLayout = imageLayout;
	imageInfos.imageView = imageView;
	imageInfos.sampler = sampler;
		
	

	return imageInfos;
}


/**
 * @brief Creates a vector of descriptor image info objects with a provided image per frame and a sampler utilized in a write descriptor set object.
 * If the size of the imageViews vector is not equal to the amount of frames in flight, the function will break.
 * 
 * @param imageLayout The layout of the provided images in the image view.
 * @param imageViews The imageview objects of the provided image.
 * @param sampler The used sampler for all of the image views.
 * @return 
 */
std::vector<VkDescriptorImageInfo> PersistanceBackend::createDescriptorImageInfoPerFrameWithArray(const VkImageLayout imageLayout, std::vector<VkImageView>& imageViews, VkSampler& sampler)
{
	if (imageViews.size() != PersistanceLib::MAXFRAMESINFLIGHT) 
	{
		std::cout << "ERROR: The imageViews Vector in createDescriptorImageInfoPerFrame() function does not have the right amount of VkImageView objects.";
		std::cout << "Got: "<< imageViews.size()<< "Expected: "<< core.getSwapchainFramebuffers()->images.size();
		BREAK;
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
 * @brief Updates a singular descriptor set with the write descriptor set objects.
 * 
 * @param descriptorSet The updated descriptor set.
 * @param writeDescriptorSets The write descriptor sets.
 * @param writeDescriptorCount The amount of write descriptor sets.
 */
void PersistanceBackend::updateDescriptorSet(VkDescriptorSet &descriptorSet, VkWriteDescriptorSet *writeDescriptorSets, const uint32_t writeDescriptorCount)
{
	if (writeDescriptorSets == nullptr)
	{ 
		std::cout<<"Error: WriteDescriptorSets array cannot be nullptr!";
		BREAK;
	}

	for (uint32_t i = 0; i < writeDescriptorCount; i++) 
	{
		writeDescriptorSets[i].dstSet = descriptorSet;
	}

	vkUpdateDescriptorSets(core.getDevice(), writeDescriptorCount, writeDescriptorSets, 0, nullptr);

}

/**
 * @brief Updates the descriptor sets with the write descriptor set objects.
 *
 * @param descriptorSet The updated descriptor sets.
 * @param writeDescriptorSets The write descriptor sets used.
 * @param writeDescriptorCount The amount of write descriptor sets.
 */
void PersistanceBackend::updateDescriptorSetsPerFrame(std::vector<VkDescriptorSet>& descriptorSet, VkWriteDescriptorSet* writeDescriptorSets, const uint32_t writeDescriptorCount)
{
	for (int i = 0; i < static_cast<int>(descriptorSet.size()); i++) 
	{
		
		for (int j = 0; j < writeDescriptorCount; j++) 
		{
			writeDescriptorSets[j].dstSet = descriptorSet[i];
		}
		
		vkUpdateDescriptorSets(core.getDevice(), static_cast<uint32_t>(writeDescriptorCount), writeDescriptorSets, 0, nullptr);
		
	}

}


/**
 * @brief Updates a descriptor sets in every frame with the write descriptor set object.
 * 
 * @param descriptorSet The updated descriptor sets.
 * @param writeDescriptorSets The write descriptor sets used.

 */
void PersistanceBackend::updateDescriptorSetPerFrameWithArray(std::vector<VkDescriptorSet> &descriptorSet, PersistanceUtils::ArrayView<VkWriteDescriptorSet> writeDescriptorSets)
{
	if (writeDescriptorSets.data() == nullptr) {
		std::cout<< "Write descriptor set viewer must not be empty";
		BREAK;
	}

	for (uint32_t i = 0; i < PersistanceLib::MAXFRAMESINFLIGHT; i++) 
	{
		writeDescriptorSets[i].dstSet = descriptorSet[i];
	}
	
	vkUpdateDescriptorSets(core.getDevice(), PersistanceLib::MAXFRAMESINFLIGHT, writeDescriptorSets.data(), 0, nullptr);

}


