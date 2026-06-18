#pragma once
#include "PersistanceLib.h"
#include "../Core/CoreUtils.h"
#include <vector>

struct Shader {
	std::vector<VkShaderModule> shaderModules;
	std::vector<VkPipelineShaderStageCreateInfo> stages;

	void createShaderStage(const char* shaderPath, VkShaderStageFlagBits shaderStage);

};


