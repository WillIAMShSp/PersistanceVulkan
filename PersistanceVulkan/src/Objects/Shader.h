#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <array>



class Shader
{
public:
	Shader() = default;

	inline auto& GetModules()
	{

		return m_modules;
	}

	inline auto& GetStages()
	{
		return m_stages;

	}

	void AddVertexShaderStage(VkShaderModule& module);   //this piece of code adds a shader stage to the member array allowing the engine to access the modules and stages to use them or destroy them.
	void AddFragmentShaderStage(VkShaderModule& module);
	
	//void AddComputeShaderStage();



private:

	static const int MAXSTAGES = 3;
	
	std::array<VkShaderModule, MAXSTAGES> m_modules;
	std::array<VkPipelineShaderStageCreateInfo, MAXSTAGES> m_stages;

	bool m_hasvertexstage = false;
	bool m_hasfragmentstage = false;
	//bool m_hascomputestage = false;

	uint8_t m_stagecount;

	

	

};

