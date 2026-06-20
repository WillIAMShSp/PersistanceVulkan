#include "DescriptorSet.h"

VkWriteDescriptorSet PersistanceBackend::createWriteDescriptorSet()
{
	VkWriteDescriptorSet writeDescriptorSet;

	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	



}

std::vector<VkDescriptorBufferInfo> PersistanceBackend::createDescriptorBufferInfo()
{
	std::vector<VkDescriptorBufferInfo> bufferInfos;
	bufferInfos.resize(PersistanceLib::MAXFRAMESINFLIGHT);

	for (int i = 0; i < PersistanceLib::MAXFRAMESINFLIGHT; i++) 
	{
		bufferInfos[i].buffer


	}


}


