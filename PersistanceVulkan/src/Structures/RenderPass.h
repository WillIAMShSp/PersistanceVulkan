#pragma once
#include "PersistanceLib.h"
#include "RenderPassAttachment.h"
#include <Vector>



struct RenderPass 
{

	VkRenderPass renderpass;
	
	std::vector<RenderPassAttachment> attachments;

	std::vector<RenderPassAttachment> colorattachments;
	
	std::vector<RenderPassAttachment> depthstencilattachments;
	
	std::vector<RenderPassAttachment> inputattachments;
	
	std::vector<VkSubpassDescription> subpassdescription;

	std::vector<VkSubpassDependency> subpassdependencies;


};



