#pragma once
#include "PersistanceLib.h"
#include <vector>
#include "../Objects/Shader.h"

struct GraphicsPipeline {
	
	GraphicsPipeline() = default;


	VkPipeline pipeline;

	VkPipelineLayout layout;

	Shader shader;


};

