#include "ComputeShader.h"

/**
 * @brief Creates a compute pipeline for a compute shader.
 * 
 * @param layout The pipeline layout.
 * @param stage The ShaderStageCreateInfo struct for the shader.
 * @return The compute pipeline.
 */
VkPipeline PersistanceBackend::createComputePipeline(VkPipelineLayout &layout, VkPipelineShaderStageCreateInfo stage)
{
    VkPipeline pipeline;

    VkComputePipelineCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    info.layout = layout;
    info.stage = stage;

    if (vkCreateComputePipelines(core.getDevice(), VK_NULL_HANDLE, 1, &info, nullptr, &pipeline) != VK_SUCCESS) {
        std::cout<< "Failed to create compute pipeline!\n";
    }

    return pipeline;

}


/**
 * @brief Dispatches the bound compute shader.
 * 
 * @param commandBuffer Command buffer to dispatch the shader on.
 * @param x The amount of work groups on the x axis.
 * @param y The amount of work groups on the y axis.
 * @param z The amount of work groups on the z axis.
 */
void PersistanceBackend::dispatchComputeShader(VkCommandBuffer &commandBuffer, size_t x, size_t y, size_t z)
{
    vkCmdDispatch(commandBuffer, x, y, z);
}


