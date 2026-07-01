/*****************************************************************//**
 * @file   BackendShader.cpp
 * @brief  The Class definition for PersistanceBackend::Shader
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/

#include "../Backend/Shader.h"

/**
 * @brief Creates a shader stage within the shader object.
 * 
 * @param shaderPath The path of the shader .spv
 * @param shaderStage Which part of the shader pipeline is this stage from (i.e vertex shader, fragment shader, ...)
 */
void PersistanceBackend::Shader::createShaderStage(const char* shaderPath, VkShaderStageFlagBits shaderStage)
{
	std::vector<char> buffer = PersistanceUtils::readFile(shaderPath);

	uint32_t moduleID = shaderModules.size();
	shaderModules.push_back(PersistanceUtils::createShaderModule(buffer));

	VkPipelineShaderStageCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	info.module = shaderModules.at(moduleID);
	info.pName = "main";
	info.stage = shaderStage;

	stages.push_back(info);




}