#include "ComputeShader.h"

void PersistanceBackend::DispatchComputeShader(VkCommandBuffer &commandBuffer, size_t x, size_t y, size_t z)
{
    vkCmdDispatch(commandBuffer, x, y, z);
}