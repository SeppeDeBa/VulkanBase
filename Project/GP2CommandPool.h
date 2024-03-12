#pragma once
#include "vulkan/vulkan_core.h"
#include "vulkanbase/VulkanUtil.h"

#include "GP2CommandBuffer.h"

class GP2CommandPool
{
public:
	GP2CommandPool()
		:m_CommandPool(VK_NULL_HANDLE)
		,m_VkDevice(VK_NULL_HANDLE)
	{}
	~GP2CommandPool() = default;

	void Initialize(const VkDevice& device, const QueueFamilyIndices& queue);
	void Destroy();

	GP2CommandBuffer createCommandBuffer() const;
		
private:
	VkCommandPool m_CommandPool;
	VkDevice m_VkDevice;
	
};