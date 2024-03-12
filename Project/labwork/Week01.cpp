#include "vulkanbase/VulkanBase.h"

void VulkanBase::initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}



void VulkanBase::drawScene() {

	VkBuffer vertexBuffers[] = { vertexBuffer.GetVertexBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer.getVkCommandBuffer(), 0, 1, vertexBuffers , offsets);
	

	//VkBuffer vertexBuffers[] = { vertexBuffer };
	//VkDeviceSize offsets[] = { 0 };

	vkCmdDraw(commandBuffer.getVkCommandBuffer(), static_cast<uint32_t>(mesh.GetVertices().size()), 1, 0, 0);

	//vkCmdDraw(commandBuffer.getVkCommandBuffer(), 6, 1, 0, 0);
}