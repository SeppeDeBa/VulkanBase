#pragma once
#include "vulkan/vulkan_core.h"

class GP2CommandBuffer
{
public:
	GP2CommandBuffer() = default;
	~GP2CommandBuffer() = default;

	void setVkCommandBuffer(VkCommandBuffer buffer)
	{
		m_CommandBuffer = buffer;
	}

	VkCommandBuffer GetVkCommandBuffer()
	{
		return m_CommandBuffer;
	}

	void reset()const;
	void beginRecording()const;
	void endRecording()const;
	
	void submit(VkSubmitInfo & info)const;

private:
	VkCommandBuffer m_CommandBuffer;
};