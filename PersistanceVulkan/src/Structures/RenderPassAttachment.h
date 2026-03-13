#pragma once

#include "PersistanceLib.h"

struct RenderPassAttachment 
{
	              
	AttachmentDescription description;

	AttachmentReference reference;


};

struct AttachmentDescription 
{

	VkFormat format;
	VkSampleCountFlags samples;
	VkAttachmentLoadOp loadop;
	VkAttachmentStoreOp storeop;
	VkImageLayout initiallayout = VK_IMAGE_LAYOUT_UNDEFINED;
	VkImageLayout finallayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;




};

struct AttachmentReference 
{
	uint32_t attachment;
	VkImageLayout layout;


}; 