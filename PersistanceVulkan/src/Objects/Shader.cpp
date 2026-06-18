#include "Shader.h"







void Shader::AddVertexShaderStage()
{
	int index = m_stages.size();
	
	m_stages.push_back(VkPipelineShaderStageCreateInfo());
	
	VkPipelineShaderStageCreateInfo& createinfo = m_stages[index];
	
	createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createinfo.module = m_vertexmodule;
	createinfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	createinfo.pName = "main";


	if (!m_hasvertexstage)
	{
		m_hasfragmentstage = true;
		m_stagecount++;
	}

}

void Shader::AddFragmentShaderStage()
{
	int index = m_stages.size();

	m_stages.push_back(VkPipelineShaderStageCreateInfo());

	VkPipelineShaderStageCreateInfo& createinfo = m_stages[index];
	createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createinfo.module = m_fragmentmodule;
	createinfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	createinfo.pName = "main";

	if (!m_hasfragmentstage)
	{
		m_hasfragmentstage = true;
		m_stagecount++;

	}
}


