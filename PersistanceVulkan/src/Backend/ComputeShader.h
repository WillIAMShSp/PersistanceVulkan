#pragma once 
#include "PersistanceLib.h"
#include "Core/PersistanceVkCore.h"
#include "Structures/Buffer.h"
#include "Backend/Shader.h"

namespace PersistanceBackend 
{

    VkPipeline createComputePipeline(VkPipelineLayout &layout, Shader& shader);
 
    void dispatchComputeShader(VkCommandBuffer& commandBuffer, size_t x, size_t y, size_t z);


}
