#pragma once
#include "PersistanceLib.h"
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



struct VertexInputState
{
	VkFormat format;
	uint32_t offset;
};
struct VertexInputStateLayout
{


	std::vector<VertexInputState> m_attributes{};

	uint32_t stride = 0;

	template<typename T>
	void push()
	{

	}
	template<>
	void push<glm::vec4>()
	{
		m_attributes.push_back({ VK_FORMAT_R32G32B32A32_SFLOAT, stride });
		stride += sizeof(glm::vec4);
	}
	template<>
	void push<glm::vec3>()
	{
		m_attributes.push_back({ VK_FORMAT_R32G32B32_SFLOAT, stride });
		stride += sizeof(glm::vec3);
	}
	template<>
	void push<glm::vec2>()
	{
		m_attributes.push_back({ VK_FORMAT_R32G32_SFLOAT, stride });
		stride += sizeof(glm::vec2);
	}
	template<>
	void push<float>()
	{
		m_attributes.push_back({VK_FORMAT_R32_SFLOAT, stride});
		stride += sizeof(float);
	}


	

};



class PipelineSettings
{
public:
	PipelineSettings() = default;
	
	void createVertexInputState(VertexInputStateLayout& layout);
	void defineInputAssemblyState(VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VkBool32 primitiverestart = VK_FALSE);
	void createStaticViewPortAndScissors(uint32_t xPos, uint32_t yPos, float minDepth, float maxDepth, VkExtent2D viewportExtent, VkExtent2D scissorsExtent, VkOffset2D scissorsOffset);
	void configureRasterizationStage();
	void configureRasterizationStage( 
		VkPolygonMode polygonmode, 
		VkCullModeFlagBits cullmode, 
		VkFrontFace frontface,
		VkBool32 enabledepthclamp = VK_FALSE,
		float linewidth = 1.0f,
		VkBool32 rasterizerdiscard = VK_FALSE
	);
	void configureRasterizationDepthBias(VkBool32 depthbiasenable, VkBool32 depthbiasclamp, float depthbiasslopefactor, float depthbiasconstantfactor);
	void configureMultisample();
	void configureMultisample(
		VkBool32 sampleshading,
		VkSampleCountFlagBits rasterizationsamples,
		float minsampleshading = 1.0f,
		VkSampleMask* samplemask = nullptr,
		VkBool32 alphatocoverage = VK_FALSE,
		VkBool32 alphatoone = VK_FALSE
	);
	void configureColorBlend();
	void configureColorBlend(
		VkColorComponentFlagBits colorcomponents, 
		VkBool32 enableblend, 
		VkBlendOp colorblend, 
		VkBlendOp alphablend, 
		VkBlendFactor srccolorblendfactor,
		VkBlendFactor dstcolorblendfactor
		);

	void useDynamicViewport(); 
	

public:
	const VkPipelineVertexInputStateCreateInfo& getVertexInputStateCreateInfo()
	{
		return m_vertexInputStateCreateInfo;
	}
	const VkPipelineInputAssemblyStateCreateInfo& getInputAssemblyStateCreateInfo()
	{
		return m_inputAssemblyStateCreateInfo;
	}
	const VkPipelineViewportStateCreateInfo& getViewportCreateInfo()
	{
		return m_viewportCreateInfo;
	}
	const VkPipelineRasterizationStateCreateInfo& getRasterCreateInfo()
	{
		return m_rasterCreateInfo;
	}
	const VkPipelineMultisampleStateCreateInfo& getMultisampleCreateInfo()
	{
		return m_multisampleCreateInfo;
	}
	const VkPipelineColorBlendStateCreateInfo& getColorBlendCreateInfo()
	{
		return m_colorBlendCreateInfo;
	}
	
	bool m_usedynamicstate = false;

private:
	std::vector<VkVertexInputAttributeDescription> m_attributeDescription{};
	VkVertexInputBindingDescription m_bindingDescription{};
	VkPipelineVertexInputStateCreateInfo m_vertexInputStateCreateInfo{};
	VkPipelineInputAssemblyStateCreateInfo m_inputAssemblyStateCreateInfo{};
	VkViewport m_staticViewport;
	VkRect2D m_staticScissors;
	VkPipelineViewportStateCreateInfo m_viewportCreateInfo{};
	VkPipelineRasterizationStateCreateInfo m_rasterCreateInfo{};
	VkPipelineMultisampleStateCreateInfo m_multisampleCreateInfo{};
	VkPipelineColorBlendAttachmentState m_colorBlendAttachment{};
	VkPipelineColorBlendStateCreateInfo m_colorBlendCreateInfo{};
	

};