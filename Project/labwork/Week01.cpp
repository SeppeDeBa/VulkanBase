#include "vulkanbase/VulkanBase.h"
void VulkanBase::initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);


	// glfwSetWindowUserPointer(window, this);
	//[](GLFWwindow* window, int key, int scancode, int action, int mods) //-> cannot capture data.
	//[](GLFWwindow*, double xpos, double ypos)//
	//no mouse movement event
	//why not normalize? Theres a problem when normals point in different directions.


}

void VulkanBase::drawScene() {

	VkBuffer vertexBuffers[] = { vertexBuffer.GetBuffer() };
	VkBuffer indexBuffers = { indexBuffer.GetBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer.GetVkCommandBuffer(), 0, 1, vertexBuffers, offsets);


	vkCmdBindIndexBuffer(commandBuffer.GetVkCommandBuffer(), indexBuffers, 0, VK_INDEX_TYPE_UINT16);

	//VkBuffer vertexBuffers[] = { vertexBuffer };
	//VkDeviceSize offsets[] = { 0 };

	vkCmdDrawIndexed(commandBuffer.GetVkCommandBuffer(), static_cast<uint32_t>(mesh.GetIndices().size()), 1, 0, 0, 0);

	//vkCmdDraw(commandBuffer.getVkCommandBuffer(), 6, 1, 0, 0);
}

//got idea to make a 3DDrawScene from Siebe Boeckx, was only idea, implemented by myself

void VulkanBase::draw3DScene(uint32_t imageIndex)

{	VkBuffer vertexBuffers[] = { vertexBuffer.GetBuffer() };
	VkBuffer indexBuffers = { indexBuffer.GetBuffer() };
	VkDeviceSize offsets[] = { 0 };
	//bind all buffers
	vkCmdBindVertexBuffers(commandBuffer.GetVkCommandBuffer(), 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer.GetVkCommandBuffer(), indexBuffers, 0, VK_INDEX_TYPE_UINT16);
	vkCmdBindDescriptorSets(commandBuffer.GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorPool.GetDescriptorSets()[imageIndex], 0, nullptr);
	vkCmdDrawIndexed(commandBuffer.GetVkCommandBuffer(), static_cast<uint32_t>(mesh.GetIndices().size()), 1, 0, 0, 0);

	//vkCmdDraw(commandBuffer.getVkCommandBuffer(), 6, 1, 0, 0);
	
}