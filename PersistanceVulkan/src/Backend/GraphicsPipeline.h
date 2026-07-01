/*****************************************************************//**
 * @file   GraphicsPipeline.h
 * @brief  Function declaration for graphics pipeline creation and destruction.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/

#pragma once
#include "PersistanceLib.h"
#include "../Core/PersistanceVkCore.h"
#include "../Structures/GraphicsPipeline.h"
#include "./Shader.h"
#include "../Settings/PipelineSettings.h"
#include "../Structures/RenderPass.h"


namespace PersistanceBackend 
{

	VkPipelineLayout createPipelineLayout(const VkDescriptorSetLayout* layouts, const uint32_t layoutCount, const VkPushConstantRange* pushConstantRanges, const uint32_t pushConstangRangeCount);

	VkPipeline createGraphicsPipeline(VkPipelineLayout& layout, Shader& shader, PipelineSettings& settings, RenderPass& renderPass);

	void cleanUpGraphicsPipeline(VkPipeline* pipelines, const uint32_t pipelineCount);

}




