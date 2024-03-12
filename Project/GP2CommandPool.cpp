#include "GP2CommandPool.h"

void GP2CommandPool::Initialize(const VkDevice& device, const QueueFamilyIndices& queue)
{
	m_VkDevice = device;

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queue.graphicsFamily.value();

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

void GP2CommandPool::Destroy()
{
	vkDestroyCommandPool(m_VkDevice, m_CommandPool, nullptr);
}

GP2CommandBuffer GP2CommandPool::createCommandBuffer() const
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_CommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;

	if (vkAllocateCommandBuffers(m_VkDevice, &allocInfo, &commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	GP2CommandBuffer cmdBuffer;
	cmdBuffer.setVkCommandBuffer(commandBuffer);

	return cmdBuffer;
}
