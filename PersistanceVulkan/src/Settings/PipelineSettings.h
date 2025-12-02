#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
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
	
	void CreateVertexInputState(VertexInputStateLayout& layout);
	void DefineInputAssemblyState(VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VkBool32 primitiverestart = VK_FALSE);
	void CreateStaticViewPort();
	void ConfigureRasterizationStage();
	void ConfigureRasterizationStage( 
		VkPolygonMode polygonmode, 
		VkCullModeFlagBits cullmode, 
		VkFrontFace frontface,
		VkBool32 enabledepthclamp = VK_FALSE,
		float linewidth = 1.0f,
		VkBool32 rasterizerdiscard = VK_FALSE
	);
	void ConfigureRasterizationDepthBias(VkBool32 depthbiasenable, VkBool32 depthbiasclamp, float depthbiasslopefactor, float depthbiasconstantfactor);
	void ConfigureMultisample();
	void ConfigureMultisample(
		VkBool32 sampleshading,
		VkSampleCountFlagBits rasterizationsamples,
		float minsampleshading = 1.0f,
		VkSampleMask* samplemask = nullptr,
		VkBool32 alphatocoverage = VK_FALSE,
		VkBool32 alphatoone = VK_FALSE
	);
	void ConfigureColorBlend();
	void ConfigureColorBlend(
		VkColorComponentFlagBits colorcomponents, 
		VkBool32 enableblend, 
		VkBlendOp colorblend, 
		VkBlendOp alphablend, 
		VkBlendFactor srccolorblendfactor,
		VkBlendFactor dstcolorblendfactor
		);

	void UseDynamicViewport(); 
	

public:
	const VkPipelineVertexInputStateCreateInfo& GetVertexInputStateCreateInfo()
	{
		return m_vertexinputstatecreateinfo;
	}
	const VkPipelineInputAssemblyStateCreateInfo& GetInputAssemblyStateCreateInfo()
	{
		return m_inputassemblystatecreateinfo;
	}
	const VkPipelineViewportStateCreateInfo& Getviewportcreateinfo()
	{
		return m_viewportcreateinfo;
	}
	const VkPipelineRasterizationStateCreateInfo& GetRasterCreateInfo()
	{
		return m_rastercreateinfo;
	}
	const VkPipelineMultisampleStateCreateInfo& GetMultisampleCreateInfo()
	{
		return m_multisamplecreateinfo;
	}
	const VkPipelineColorBlendStateCreateInfo& GetColorBlendCreateInfo()
	{
		return m_colorblendcreateinfo;
	}
	
	bool m_usedynamicstate = false;

private:
	std::vector<VkVertexInputAttributeDescription> m_attributedescription{};
	VkVertexInputBindingDescription m_bindingdescription{};
	VkPipelineVertexInputStateCreateInfo m_vertexinputstatecreateinfo{};
	VkPipelineInputAssemblyStateCreateInfo m_inputassemblystatecreateinfo{};
	VkPipelineViewportStateCreateInfo m_viewportcreateinfo{};
	VkPipelineRasterizationStateCreateInfo m_rastercreateinfo{};
	VkPipelineMultisampleStateCreateInfo m_multisamplecreateinfo{};
	VkPipelineColorBlendAttachmentState m_colorblendattachment{};
	VkPipelineColorBlendStateCreateInfo m_colorblendcreateinfo{};
	

};