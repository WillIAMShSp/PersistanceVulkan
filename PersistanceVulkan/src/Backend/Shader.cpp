#include "./Shader.h"


void Shader::createShaderStage(const char* shaderPath, VkShaderStageFlagBits shaderStage)
{
	std::vector<char> buffer = PersistanceUtils::readFile(shaderPath);

	uint32_t moduleID = shaderModules.size();
	shaderModules.push_back(PersistanceUtils::createShaderModule(buffer));

	VkPipelineShaderStageCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	info.module = shaderModules.at(moduleID);
	info.pName = "main";
	info.stage = shaderStage;




}
