#include "GP2CommandPool.h"

#include "GP2DataBuffer.h"

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

std::vector<GP2CommandBuffer> GP2CommandPool::createCommandBuffers() const
{
	std::vector<VkCommandBuffer> commandBuffers{};
	commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_CommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();


	if (vkAllocateCommandBuffers(m_VkDevice, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	std::vector<GP2CommandBuffer> cmdBuffers{};
	cmdBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	for(uint32_t i{}; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		cmdBuffers[i].setVkCommandBuffer(commandBuffers[i]);
	}

	return cmdBuffers;
}
