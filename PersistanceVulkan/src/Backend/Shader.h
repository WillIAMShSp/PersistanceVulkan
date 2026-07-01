/*****************************************************************//**
 * @file   Shader.h
 * @brief  Class declaration for shader creation.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/

#pragma once
#include "PersistanceLib.h"
#include "../Core/CoreUtils.h"
#include <vector>

namespace PersistanceBackend {

	/**
	 * @brief The backend implementation of shaders.
	 */
	struct Shader {
		std::vector<VkShaderModule> shaderModules;
		std::vector<VkPipelineShaderStageCreateInfo> stages;

		void createShaderStage(const char* shaderPath, VkShaderStageFlagBits shaderStage);

	};
}

