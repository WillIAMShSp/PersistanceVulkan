#pragma once
#include "PersistanceLib.h"
#include <vector>

class Drawable {

public:

	Drawable() = default;

	std::vector<uint32_t>& GetVertexBufferHandles() 
	{
		return m_vertexbufferhandles;
	}
	std::vector<VkDeviceSize>& GetVertexBufferOffsets() 
	{
		return m_vertexoffsets;
	}
	uint32_t GetIndexBufferHandle()
	{
		return m_indexbufferhandle;
	}
	uint32_t GetDescriptorSetHandle() 
	{
		return m_descriptorsethandle;
	}
	uint32_t GetGraphicsPipelineHandle() 
	{
		return m_graphicspipelinehandle;
	}
	VkPipelineBindPoint GetGraphicsPipelineBindingPoint() 
	{
		return m_bindingpoint;
	}

	void AddVertexBuffer(uint32_t handle) 
	{
		m_vertexbufferhandles.push_back(handle);
	}
	void AddVertexBufferOffset(VkDeviceSize offset) 
	{
		m_vertexoffsets.push_back(offset);
	}
	void SetIndexBufferHandle(uint32_t handle) 
	{
		m_indexbufferhandle = handle;
	}
	void SetDescriptorSetHandle(uint32_t handle) 
	{
		m_descriptorsethandle = handle;
	}
	void SetGraphicsPipelineHandle(uint32_t handle) 
	{
		m_graphicspipelinehandle = handle;
	}

private:
	std::vector<uint32_t>m_vertexbufferhandles;
	std::vector<VkDeviceSize> m_vertexoffsets;
	uint32_t m_indexbufferhandle;
	uint32_t m_descriptorsethandle;
	uint32_t m_graphicspipelinehandle;


	VkPipelineBindPoint m_bindingpoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	
	


};
