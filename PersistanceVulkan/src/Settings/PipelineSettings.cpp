#include "PipelineSettings.h"

void PipelineSettings::createVertexInputState(VertexInputStateLayout& layout)
{

	
	m_attributeDescription.resize(layout.m_attributes.size());
	for (int i = 0; i < m_attributeDescription.size(); i++)
	{
		VertexInputState attribute = layout.m_attributes[i];

		m_attributeDescription[i].binding = 0;
		m_attributeDescription[i].location = i;
		m_attributeDescription[i].format = attribute.format;
		m_attributeDescription[i].offset = attribute.offset;

	}
	
	
	m_bindingDescription.binding = 0;
	m_bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	m_bindingDescription.stride = layout.stride;


	m_vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	m_vertexInputStateCreateInfo.pVertexAttributeDescriptions = m_attributeDescription.data();
	m_vertexInputStateCreateInfo.pVertexBindingDescriptions = &m_bindingDescription;
	m_vertexInputStateCreateInfo.vertexAttributeDescriptionCount = m_attributeDescription.size();
	m_vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
	


}

void PipelineSettings::defineInputAssemblyState(VkPrimitiveTopology topology, VkBool32 primitiverestart)
{
	m_inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	m_inputAssemblyStateCreateInfo.topology = topology;
	m_inputAssemblyStateCreateInfo.primitiveRestartEnable = primitiverestart;


}

void PipelineSettings::createStaticViewPortAndScissors(uint32_t xPos, uint32_t yPos, float minDepth, float maxDepth, VkExtent2D viewportExtent, VkExtent2D scissorsExtent, VkOffset2D scissorsOffset)
{
	m_staticViewport.x = xPos;
	m_staticViewport.y = yPos;
	m_staticViewport.minDepth = minDepth;
	m_staticViewport.maxDepth = maxDepth;
	m_staticViewport.width = (float)viewportExtent.width;
	m_staticViewport.height = (float)viewportExtent.height;
	
	m_staticScissors.extent = scissorsExtent;
	m_staticScissors.offset = scissorsOffset;


	m_viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	m_viewportCreateInfo.viewportCount = 1;
	m_viewportCreateInfo.scissorCount = 1;
	m_viewportCreateInfo.pViewports = &m_staticViewport;
	m_viewportCreateInfo.pScissors = &m_staticScissors;
}

void PipelineSettings::configureRasterizationStage()
{
	m_rasterCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	m_rasterCreateInfo.depthClampEnable = VK_FALSE;
	m_rasterCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	m_rasterCreateInfo.lineWidth = 1.0f;
	m_rasterCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	m_rasterCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	m_rasterCreateInfo.rasterizerDiscardEnable = VK_FALSE;

	m_rasterCreateInfo.depthBiasClamp = VK_FALSE;
	m_rasterCreateInfo.depthBiasEnable = VK_FALSE;
	m_rasterCreateInfo.depthBiasSlopeFactor = 0.f;
	m_rasterCreateInfo.depthBiasConstantFactor = 0.f;
}

void PipelineSettings::configureRasterizationStage(VkPolygonMode polygonmode, VkCullModeFlagBits cullmode, VkFrontFace frontface, VkBool32 enabledepthclamp, float linewidth, VkBool32 rasterizerdiscard)
{
	m_rasterCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	m_rasterCreateInfo.depthClampEnable = enabledepthclamp;
	m_rasterCreateInfo.polygonMode = polygonmode;
	m_rasterCreateInfo.lineWidth = linewidth;
	m_rasterCreateInfo.cullMode = cullmode;
	m_rasterCreateInfo.frontFace = frontface;
	m_rasterCreateInfo.rasterizerDiscardEnable = rasterizerdiscard;

	m_rasterCreateInfo.depthBiasClamp = VK_FALSE;
	m_rasterCreateInfo.depthBiasEnable = VK_FALSE;
	m_rasterCreateInfo.depthBiasSlopeFactor = 0.f;
	m_rasterCreateInfo.depthBiasConstantFactor = 0.f;


}

void PipelineSettings::configureRasterizationDepthBias(VkBool32 depthbiasenable, VkBool32 depthbiasclamp, float depthbiasslopefactor, float depthbiasconstantfactor)
{
	m_rasterCreateInfo.depthBiasClamp = depthbiasclamp;
	m_rasterCreateInfo.depthBiasEnable = depthbiasenable;
	m_rasterCreateInfo.depthBiasSlopeFactor = depthbiasslopefactor;
	m_rasterCreateInfo.depthBiasConstantFactor = depthbiasconstantfactor;

}

void PipelineSettings::configureDepthStencilState(VkBool32 depthTestEnable, VkBool32 depthWriteEnable, VkCompareOp depthCompareOp, VkBool32 depthBoundsTestEnable, float minDepthBounds, float maxDepthBounds, VkBool32 stencilTestEnable, VkStencilOpState front, VkStencilOpState back)
{
	m_depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	m_depthStencilCreateInfo.depthTestEnable = depthTestEnable;
	m_depthStencilCreateInfo.depthWriteEnable = depthWriteEnable;
	m_depthStencilCreateInfo.depthCompareOp = depthCompareOp;
	m_depthStencilCreateInfo.depthBoundsTestEnable = depthBoundsTestEnable ;
	m_depthStencilCreateInfo.minDepthBounds = minDepthBounds;
	m_depthStencilCreateInfo.maxDepthBounds = maxDepthBounds;
	m_depthStencilCreateInfo.stencilTestEnable = stencilTestEnable;
	m_depthStencilCreateInfo.front = front;
	m_depthStencilCreateInfo.back = back;


	m_pDepthStencilCreateInfo = &m_depthStencilCreateInfo;

}

void PipelineSettings::configureMultisample()
{
	m_multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	m_multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
	m_multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	m_multisampleCreateInfo.minSampleShading = 1.0f;
	m_multisampleCreateInfo.pSampleMask = nullptr;
	m_multisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
	m_multisampleCreateInfo.alphaToOneEnable = VK_FALSE;

}

void PipelineSettings::configureMultisample(VkBool32 sampleshading, VkSampleCountFlagBits rasterizationsamples, float minsampleshading, VkSampleMask* samplemask, VkBool32 alphatocoverage, VkBool32 alphatoone)
{
	m_multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	m_multisampleCreateInfo.sampleShadingEnable = sampleshading;
	m_multisampleCreateInfo.rasterizationSamples = rasterizationsamples;
	m_multisampleCreateInfo.minSampleShading = minsampleshading;
	m_multisampleCreateInfo.pSampleMask = samplemask;
	m_multisampleCreateInfo.alphaToCoverageEnable = alphatocoverage;
	m_multisampleCreateInfo.alphaToOneEnable = alphatoone;


}

void PipelineSettings::configureColorBlend()
{
	

	m_colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	m_colorBlendAttachment.blendEnable = VK_TRUE;

	m_colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	m_colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	m_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	m_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

	m_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	m_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

	m_colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	m_colorBlendCreateInfo.attachmentCount = 1;
	m_colorBlendCreateInfo.logicOpEnable = VK_FALSE;
	m_colorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	m_colorBlendCreateInfo.pAttachments = &m_colorBlendAttachment;
	m_colorBlendCreateInfo.blendConstants[0] = 0.0f;
	m_colorBlendCreateInfo.blendConstants[1] = 0.0f;
	m_colorBlendCreateInfo.blendConstants[2] = 0.0f;
	m_colorBlendCreateInfo.blendConstants[3] = 0.0f;
	//TODO: this might have to do with each color, I wonder what happens if we dont have them all activated. I'll test that... eventually.

}

void PipelineSettings::configureColorBlend(VkColorComponentFlagBits colorcomponents, VkBool32 enableblend, VkBlendOp colorblend, VkBlendOp alphablend, VkBlendFactor srccolorblendfactor, VkBlendFactor dstcolorblendfactor)
{
	
	m_colorBlendAttachment.colorWriteMask = colorcomponents;

	m_colorBlendAttachment.blendEnable = enableblend;

	m_colorBlendAttachment.colorBlendOp = colorblend;
	m_colorBlendAttachment.alphaBlendOp = alphablend;

	m_colorBlendAttachment.srcColorBlendFactor = srccolorblendfactor;
	m_colorBlendAttachment.dstColorBlendFactor = dstcolorblendfactor;
	//TODO: I want to change how colorblend and alpha blend factors are controlled. I want to have the posibility to have a couple of commonly used options
	// as enums, and that the programer can just use one of those enums.
	m_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	m_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

	m_colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	m_colorBlendCreateInfo.attachmentCount = 1;
	m_colorBlendCreateInfo.logicOpEnable = VK_FALSE;
	m_colorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	m_colorBlendCreateInfo.pAttachments = &m_colorBlendAttachment;
	m_colorBlendCreateInfo.blendConstants[0] = 0.0f;
	m_colorBlendCreateInfo.blendConstants[1] = 0.0f;
	m_colorBlendCreateInfo.blendConstants[2] = 0.0f;
	m_colorBlendCreateInfo.blendConstants[3] = 0.0f;

}

void PipelineSettings::useDynamicViewport()
{
	m_usedynamicstate = true;
}



