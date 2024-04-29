#include "GP2CommandBuffer.h"
#include <stdexcept>

void GP2CommandBuffer::reset() const
{
	vkResetCommandBuffer(m_CommandBuffer, /*VkCommandBufferResetFlagBits*/ 0);
}

void GP2CommandBuffer::beginRecording() const
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional
	
	if (vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");	
	}
}

void GP2CommandBuffer::endRecording() const
{
	if (vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

void GP2CommandBuffer::submit(VkSubmitInfo& info) const
{
	info.commandBufferCount = 1;
	info.pCommandBuffers = &m_CommandBuffer;
}
