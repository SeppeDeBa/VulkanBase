#include "vulkanbase/VulkanBase.h"
void VulkanBase::initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}



void VulkanBase::drawScene() {

	VkBuffer vertexBuffers[] = { vertexBuffer.GetBuffer() };
	VkBuffer indexBuffers = { indexBuffer.GetBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer.GetVkCommandBuffer(), 0, 1, vertexBuffers , offsets);


	vkCmdBindIndexBuffer(commandBuffer.GetVkCommandBuffer(), indexBuffers, 0, VK_INDEX_TYPE_UINT16);

	//VkBuffer vertexBuffers[] = { vertexBuffer };
	//VkDeviceSize offsets[] = { 0 };

	vkCmdDrawIndexed(commandBuffer.GetVkCommandBuffer(), static_cast<uint32_t>(mesh.GetVertices().size()), 1, 0, 0, 0);

	//vkCmdDraw(commandBuffer.getVkCommandBuffer(), 6, 1, 0, 0);
}