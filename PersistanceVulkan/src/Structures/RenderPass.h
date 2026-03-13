#pragma once
#include "PersistanceLib.h"
#include "RenderPassAttachment.h"
#include <Vector>

struct RenderPass 
{

	VkRenderPass renderpass;
	
	std::vector<RenderPassAttachment> attachments;
	
	std::vector<Subpass> subpasses;

	std::vector<SubpassDependency> dependencies;


};

struct SubpassDependency {

	uint32_t srcsubpass = VK_SUBPASS_EXTERNAL;
	uint32_t dstsubpass = 0;

	VkPipelineStageFlagBits srcstagemask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkAccessFlags srcaccessmask = 0;

	VkPipelineStageFlagBits dststagemask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkAccessFlags dstaccessmask = 0;



};

struct Subpass 
{
	uint32_t colorattachmentcount;
	VkAttachmentReference& attachmentreference;

};

