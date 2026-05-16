#pragma once
#include "PersistanceLib.h"
#include <vector>
#include "../Objects/Shader.h"

typedef uint32_t GraphicsPipelineHandle;

struct GraphicsPipeline {
	
	GraphicsPipeline() = default;


	VkPipeline pipeline;

	VkPipelineLayout layout;

	Shader shader;


};

