#pragma once

#include "PersistanceLib.h"
#include <vector>

struct RenderPassAttachment 
{

	VkAttachmentDescription description;

	VkAttachmentReference reference;

};


struct AttachmentReferenceList {

	void add(RenderPassAttachment attachment)
	{
		references.push_back(attachment.reference);
	};
	void add(const RenderPassAttachment* attachments, uint32_t attachmentCount)
	{
		references.reserve(attachmentCount + references.size());

		for (uint32_t i = 0; i < attachmentCount; i++) 
		{
			references.emplace_back(attachments[i].reference);
			
		}
	}

	std::vector<VkAttachmentReference> references;

};

struct AttachmentDescriptionList {
	void add(RenderPassAttachment* attachments, uint32_t attachmentCount) 
	{
		descriptions.reserve(attachmentCount + descriptions.size());
	}

	std::vector<VkAttachmentDescription> descriptions;
};
 