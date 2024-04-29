#include "vulkanbase/VulkanBase.h"

void VulkanBase::initWindow() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		void* pUser = glfwGetWindowUserPointer(window);
		VulkanBase* vBase = static_cast<VulkanBase*>(pUser);
		vBase->keyEvent(key, scancode, action, mods);
		});
	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
		void* pUser = glfwGetWindowUserPointer(window);
		VulkanBase* vBase = static_cast<VulkanBase*>(pUser);
		vBase->mouseMove(window, xpos, ypos);
		});
	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
		void* pUser = glfwGetWindowUserPointer(window);
		VulkanBase* vBase = static_cast<VulkanBase*>(pUser);
		vBase->mouseEvent(window, button, action, mods);
		});

	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


	// glfwSetWindowUserPointer(window, this);
	//[](GLFWwindow* window, int key, int scancode, int action, int mods) //-> cannot capture data.
	//[](GLFWwindow*, double xpos, double ypos)//
	//no mouse movement event
	//why not normalize? Theres a problem when normals point in different directions.


}



void VulkanBase::drawScene() {

	/* after rework
	VkBuffer vertexBuffers[] = { vertexBuffer.GetBuffer() };
	VkBuffer indexBuffers = { indexBuffer.GetBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffers[currentFrame].GetVkCommandBuffer(), 0, 1, vertexBuffers, offsets);


	vkCmdBindIndexBuffer(commandBuffers[currentFrame].GetVkCommandBuffer(), indexBuffers, 0, VK_INDEX_TYPE_UINT16);

	//VkBuffer vertexBuffers[] = { vertexBuffer };
	//VkDeviceSize offsets[] = { 0 };

	vkCmdDrawIndexed(commandBuffers[currentFrame].GetVkCommandBuffer(), static_cast<uint32_t>(mesh.GetIndices().size()), 1, 0, 0, 0);

	//vkCmdDraw(commandBuffer.getVkCommandBuffer(), 6, 1, 0, 0);
	*/
}


void VulkanBase::draw3DScene(uint32_t imageIndex)

{
	/* after rework
	 
	 VkBuffer vertexBuffers[] = { vertexBuffer.GetBuffer() };
	VkBuffer indexBuffers = { indexBuffer.GetBuffer() };
	VkDeviceSize offsets[] = { 0 };
	//bind all buffers
	vkCmdBindVertexBuffers(commandBuffers[currentFrame].GetVkCommandBuffer(), 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffers[currentFrame].GetVkCommandBuffer(), indexBuffers, 0, VK_INDEX_TYPE_UINT16);
	vkCmdBindDescriptorSets(commandBuffers[currentFrame].GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorPool.GetDescriptorSets()[currentFrame], 0, nullptr);
	vkCmdDrawIndexed(commandBuffers[currentFrame].GetVkCommandBuffer(), static_cast<uint32_t>(mesh.GetIndices().size()), 1, 0, 0, 0);

	//vkCmdDraw(commandBuffer.getVkCommandBuffer(), 6, 1, 0, 0);
	*/
	
}