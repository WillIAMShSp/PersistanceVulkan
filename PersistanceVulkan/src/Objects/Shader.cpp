#include "Shader.h"


void Shader::AddVertexShaderStage(VkShaderModule& module)
{
	m_modules[0] = module;

	VkPipelineShaderStageCreateInfo createinfo{};
	createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createinfo.module = m_modules[0];
	createinfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	createinfo.pName = "main";

	m_stages[0] = createinfo;

	if (!m_hasvertexstage)
	{
		m_hasvertexstage = true;
		m_stagecount++;

	}
	


}

void Shader::AddFragmentShaderStage(VkShaderModule& module)
{
	m_modules[1] = module;

	VkPipelineShaderStageCreateInfo createinfo{};
	createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createinfo.module = module;
	createinfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	createinfo.pName = "main";

	m_stages[0] = createinfo;

	if (!m_hasfragmentstage)
	{
		m_hasfragmentstage = true;
		m_stagecount++;

	}

}
