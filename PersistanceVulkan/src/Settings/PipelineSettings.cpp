#include "PipelineSettings.h"

void PipelineSettings::CreateVertexInputState(VertexInputStateLayout& layout)
{

	std::vector<VkVertexInputAttributeDescription> attributedescriptions;
	attributedescriptions.resize(layout.m_attributes.size());

	for (int i = 0; i < attributedescriptions.size(); i++)
	{
		VertexInputState attribute = layout.m_attributes[i];

		attributedescriptions[i].binding = 0;
		attributedescriptions[i].location = i;
		attributedescriptions[i].format = attribute.format;
		attributedescriptions[i].offset = attribute.offset;

	}
	
	VkVertexInputBindingDescription bindingdescription{};
	bindingdescription.binding = 0;
	bindingdescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	bindingdescription.stride = layout.stride;


	m_vertexinputstatecreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	m_vertexinputstatecreateinfo.pVertexAttributeDescriptions = attributedescriptions.data();
	m_vertexinputstatecreateinfo.pVertexBindingDescriptions = &bindingdescription;
	m_vertexinputstatecreateinfo.vertexAttributeDescriptionCount = attributedescriptions.size();
	m_vertexinputstatecreateinfo.vertexBindingDescriptionCount = 1;
	


}

void PipelineSettings::DefineInputAssemblyState(VkPrimitiveTopology topology, VkBool32 primitiverestart)
{
	m_inputassemblystatecreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	m_inputassemblystatecreateinfo.topology = topology;
	m_inputassemblystatecreateinfo.primitiveRestartEnable = primitiverestart;


}

void PipelineSettings::CreateStaticViewPort()
{
	m_viewportcreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	m_viewportcreateinfo.viewportCount = 1;
	m_viewportcreateinfo.scissorCount = 1;
}

void PipelineSettings::ConfigureRasterizationStage(VkPolygonMode polygonmode, VkCullModeFlagBits cullmode, VkFrontFace frontface, VkBool32 enabledepthclamp, float linewidth, VkBool32 rasterizerdiscard)
{
	m_rastercreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	m_rastercreateinfo.depthClampEnable = enabledepthclamp;
	m_rastercreateinfo.polygonMode = polygonmode;
	m_rastercreateinfo.lineWidth = linewidth;
	m_rastercreateinfo.cullMode = cullmode;
	m_rastercreateinfo.frontFace = frontface;
	m_rastercreateinfo.rasterizerDiscardEnable = rasterizerdiscard;

	m_rastercreateinfo.depthBiasClamp = VK_FALSE;
	m_rastercreateinfo.depthBiasEnable = VK_FALSE;
	m_rastercreateinfo.depthBiasSlopeFactor = 0.f;
	m_rastercreateinfo.depthBiasConstantFactor = 0.f;


}

void PipelineSettings::ConfigureRasterizationDepthBias(VkBool32 depthbiasenable, VkBool32 depthbiasclamp, float depthbiasslopefactor, float depthbiasconstantfactor)
{
	m_rastercreateinfo.depthBiasClamp = depthbiasclamp;
	m_rastercreateinfo.depthBiasEnable = depthbiasenable;
	m_rastercreateinfo.depthBiasSlopeFactor = depthbiasslopefactor;
	m_rastercreateinfo.depthBiasConstantFactor = depthbiasconstantfactor;

}

void PipelineSettings::ConfigureMultisample(VkBool32 sampleshading, VkSampleCountFlagBits rasterizationsamples, float minsampleshading, VkSampleMask* samplemask, VkBool32 alphatocoverage, VkBool32 alphatoone)
{
	m_multisamplecreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	m_multisamplecreateinfo.sampleShadingEnable = sampleshading;
	m_multisamplecreateinfo.rasterizationSamples = rasterizationsamples;
	m_multisamplecreateinfo.minSampleShading = minsampleshading;
	m_multisamplecreateinfo.pSampleMask = samplemask;
	m_multisamplecreateinfo.alphaToCoverageEnable = alphatocoverage;
	m_multisamplecreateinfo.alphaToOneEnable = alphatoone;


}

void PipelineSettings::ConfigureColorBlend()
{
	

	m_colorblendattachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	m_colorblendattachment.blendEnable = VK_TRUE;

	m_colorblendattachment.colorBlendOp = VK_BLEND_OP_ADD;
	m_colorblendattachment.alphaBlendOp = VK_BLEND_OP_ADD;

	m_colorblendattachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	m_colorblendattachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

	m_colorblendattachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	m_colorblendattachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

	m_colorblendcreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	m_colorblendcreateinfo.attachmentCount = 1;
	m_colorblendcreateinfo.logicOpEnable = VK_FALSE;
	m_colorblendcreateinfo.logicOp = VK_LOGIC_OP_COPY;
	m_colorblendcreateinfo.pAttachments = &m_colorblendattachment;
	m_colorblendcreateinfo.blendConstants[0] = 0.0f;
	m_colorblendcreateinfo.blendConstants[1] = 0.0f;
	m_colorblendcreateinfo.blendConstants[2] = 0.0f;
	m_colorblendcreateinfo.blendConstants[3] = 0.0f;
	//TODO: this might have to do with each color, I wonder what happens if we dont have them all activated. I'll test that... eventually.

}

void PipelineSettings::ConfigureColorBlend(VkColorComponentFlagBits colorcomponents, VkBool32 enableblend, VkBlendOp colorblend, VkBlendOp alphablend, VkBlendFactor srccolorblendfactor, VkBlendFactor dstcolorblendfactor)
{
	
	m_colorblendattachment.colorWriteMask = colorcomponents;

	m_colorblendattachment.blendEnable = enableblend;

	m_colorblendattachment.colorBlendOp = colorblend;
	m_colorblendattachment.alphaBlendOp = alphablend;

	m_colorblendattachment.srcColorBlendFactor = srccolorblendfactor;
	m_colorblendattachment.dstColorBlendFactor = dstcolorblendfactor;
	//TODO: I want to change how colorblend and alpha blend factors are controlled. I want to have the posibility to have a couple of commonly used options
	// as enums, and that the programer can just use one of those enums.
	m_colorblendattachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	m_colorblendattachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

	m_colorblendcreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	m_colorblendcreateinfo.attachmentCount = 1;
	m_colorblendcreateinfo.logicOpEnable = VK_FALSE;
	m_colorblendcreateinfo.logicOp = VK_LOGIC_OP_COPY;
	m_colorblendcreateinfo.pAttachments = &m_colorblendattachment;
	m_colorblendcreateinfo.blendConstants[0] = 0.0f;
	m_colorblendcreateinfo.blendConstants[1] = 0.0f;
	m_colorblendcreateinfo.blendConstants[2] = 0.0f;
	m_colorblendcreateinfo.blendConstants[3] = 0.0f;

}

void PipelineSettings::UseDynamicViewport()
{
	std::vector<VkDynamicState> dynamicstates =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR

	};


	m_dynamicstatecreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	m_dynamicstatecreateinfo.dynamicStateCount = static_cast<uint32_t>(dynamicstates.size());
	m_dynamicstatecreateinfo.pDynamicStates = dynamicstates.data();
	m_usedynamicstate = true;

}



