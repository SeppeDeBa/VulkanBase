#include "vulkanbase/VulkanBase.h"
#include "GP2Mesh.h"

void VulkanBase::recordCommandBuffer(uint32_t imageIndex)
{
	//vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	
	drawFrame(imageIndex);
}

void VulkanBase::drawFrame(uint32_t imageIndex) {
	VkRenderPassBeginInfo renderPassInfo{}; //could be put into a function.
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapChainExtent;

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;




	//2D   ==============================================================================
	vkCmdBeginRenderPass(commandBuffers[currentFrame].GetVkCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	//record starts here


	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChainExtent.width;
	viewport.height = (float)swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffers[currentFrame].GetVkCommandBuffer(), 0, 1, &viewport);//todo: ask if needed?

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;
	vkCmdSetScissor(commandBuffers[currentFrame].GetVkCommandBuffer(), 0, 1, &scissor);//todo: ask if needed?

	//just draw both here?
	vkCmdBindPipeline(commandBuffers[currentFrame].GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline2D.GetPipeline());
	m_GraphicsPipeline2D.Record(commandBuffers, imageIndex, currentFrame);

	//record ends here



	//3D  ==============================================================================
	//needed?
	//vkCmdBeginRenderPass(commandBuffers[currentFrame].GetVkCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);


	m_GraphicsPipeline3D.Record(commandBuffers, imageIndex, currentFrame);
	//record starts here


	//VkViewport viewport{};
	//viewport.x = 0.0f;
	//viewport.y = 0.0f;
	//viewport.width = (float)swapChainExtent.width;
	//viewport.height = (float)swapChainExtent.height;
	//viewport.minDepth = 0.0f;
	//viewport.maxDepth = 1.0f;
	//vkCmdSetViewport(commandBuffers[currentFrame].GetVkCommandBuffer(), 0, 1, &viewport);

	//VkRect2D scissor{};
	//scissor.offset = { 0, 0 };
	//scissor.extent = swapChainExtent;
	//vkCmdSetScissor(commandBuffers[currentFrame].GetVkCommandBuffer(), 0, 1, &scissor);

	//just draw both here?
	vkCmdBindPipeline(commandBuffers[currentFrame].GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline3D.GetPipeline());
	draw3DScene(imageIndex);

	//record ends here

	vkCmdEndRenderPass(commandBuffers[currentFrame].GetVkCommandBuffer());




	//if (vkEndCommandBuffer(commandBuffer.GetVkCommandBuffer()) != VK_SUCCESS) {
	//	throw std::runtime_error("failed to record command buffer!");
	//}
	
}

QueueFamilyIndices VulkanBase::findQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}