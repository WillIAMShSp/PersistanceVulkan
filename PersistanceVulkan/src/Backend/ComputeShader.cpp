#include "ComputeShader.h"

void PersistanceBackend::dispatchComputeShader(VkCommandBuffer &commandBuffer, size_t x, size_t y, size_t z)
{
    vkCmdDispatch(commandBuffer, x, y, z);
}

VkPipeline PersistanceBackend::createComputeShader()
{
    return VkPipeline();
}
