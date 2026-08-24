/*****************************************************************//**
 * @file   GraphicsPipeline.h
 * @brief  Function declaration for graphics pipeline creation and destruction.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/

#pragma once
#include "../PersistanceLib.h"
#include "../Core/PersistanceVkCore.h"
#include "./Shader.h"
#include "../Settings/PipelineSettings.h"
#include "./Pipeline.h"


namespace PersistanceBackend 
{

	VkPipeline createGraphicsPipeline(VkPipelineLayout& layout, Shader& shader, PipelineSettings& settings, VkRenderPass& renderPass);

	void cleanUpGraphicsPipeline(VkPipeline* pipelines, const uint32_t pipelineCount);

}




