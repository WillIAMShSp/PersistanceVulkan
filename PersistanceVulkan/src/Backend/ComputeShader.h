#pragma once 
#include "PersistanceLib.h"
#include "Core/PersistanceVkCore.h"
#include "Structures/Buffer.h"

namespace PersistanceBackend 
{

    VkPipeline createComputePipeline(VkPipelineLayout &layout, VkPipelineShaderStageCreateInfo stage);
 
    void dispatchComputeShader(VkCommandBuffer& commandBuffer, size_t x, size_t y, size_t z);


}
