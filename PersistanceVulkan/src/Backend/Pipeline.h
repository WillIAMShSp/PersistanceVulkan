/*****************************************************************//**
 * @file   GraphicsPipeline.h
 * @brief  Function declaration for pipeline layout creation and destruction.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/
#pragma once

#include "../PersistanceLib.h"
#include "../Core/PersistanceVkCore.h"

namespace PersistanceBackend 
{
    VkPipelineLayout createPipelineLayout(const VkDescriptorSetLayout* layouts, const uint32_t layoutCount, const VkPushConstantRange* pushConstantRanges, const uint32_t pushConstangRangeCount);
    void cleanUpPipelineLayouts(VkPipelineLayout* layouts, const uint32_t layoutCount);
    
}