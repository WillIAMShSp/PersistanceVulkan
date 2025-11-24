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


	std::vector<VkVertexInputAttributeDescription> m_attributes;

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



struct PipelineSettings
{
	PipelineSettings() = default;




};