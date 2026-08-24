#pragma once 
#include "PersistanceLib.h"
#include "Structures/Buffer.h"

namespace PersistanceBackend 
{

    VkPipeline createComputeShader();
    void dispatchComputeShader(VkCommandBuffer& commandBuffer, size_t x, size_t y, size_t z);


}
