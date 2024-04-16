#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "VulkanUtil.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <optional>
#include <set>
#include <limits>
#include <algorithm>
#include "GP2Shader.h"
#include "GP2CommandPool.h"
#include "GP2Mesh.h"
#include "GP2DataBuffer.h"
#include "GP2DescriptorPool.h"
#include "GP2Pipeline.h"
const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};



class VulkanBase {

public:
static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {

	auto app = reinterpret_cast<VulkanBase*>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true;

}


	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	void initVulkan() {
		// week 06
		createInstance();
		setupDebugMessenger();
		createSurface();

		// week 05
		pickPhysicalDevice();
		createLogicalDevice();

		// week 04 
		createSwapChain();
		createImageViews();

		//TODO: REMOVE

		mesh3D1.SetVertices({ {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },
			{ {0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f} },
			{ {0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f} },
			{ {-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f} } });

		mesh3D2.SetVertices({ {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}} });

		mesh3D1.SetIndices({ 0, 1, 2, 2, 3, 0 });
		mesh3D2.SetIndices({ 4, 5, 6, 6, 7, 4 });


		
		// week 03

		//==INIT SHADERS==
		//m_GradientShader.initialize(device);
		//m_3DShader.initialize(device);
		m_GraphicsPipeline2D.Initialize(device);
		m_GraphicsPipeline3D.Initialize(device);
		//m_3DShader.CreateDescriptorSetLayout(device);

		//==RENDER PASS==
		createRenderPass();


		//==GRAPHICS PIPELINES==
		//createGraphicsPipeline();
		//createGraphicsPipeline3D();
		m_GraphicsPipeline2D.CreateGraphicsPipeline(device, renderPass);
		m_GraphicsPipeline3D.CreateGraphicsPipeline(device, renderPass);

		//==FRAME BUFFERS==
		createFrameBuffers();
		// week 02

		//==COMM POOL==
		commandPool.Initialize(device, findQueueFamilies(physicalDevice));


		//==ADD MESHES==
		m_GraphicsPipeline3D.AddMesh3D(mesh3D1, commandPool, device, physicalDevice, graphicsQueue);
		m_GraphicsPipeline3D.AddMesh3D(mesh3D2, commandPool, device, physicalDevice, graphicsQueue);

		////VBuffer
		//vertexBuffer.Initialize(device, physicalDevice, commandPool.GetCommandPool(), graphicsQueue);
		//vertexBuffer.CreateBuffer(mesh);
		////IndexBuffer
		//indexBuffer.Initialize(device, physicalDevice, commandPool.GetCommandPool(), graphicsQueue);
		//indexBuffer.CreateBuffer(mesh);
		////uniformBuffer
		//uniformBuffer.Initialize(device, physicalDevice, commandPool.GetCommandPool(), graphicsQueue);
		//uniformBuffer.CreateBuffer(mesh);
		////descriptorPool
		//descriptorPool.createDescriptorPool(device);
		//descriptorPool.createDescriptorSets(m_3DShader.GetDescriptorSetLayout(), uniformBuffer);

		//comm buffer
		commandBuffers = commandPool.createCommandBuffers();

		// week 06
		createSyncObjects();
	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			// week 06
			drawFrame();
		}
		vkDeviceWaitIdle(device);
	}

	void cleanup() {

		cleanupSwapChain();

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(device, inFlightFences[i], nullptr);
		}

		m_GraphicsPipeline2D.Cleanup(device);
		m_GraphicsPipeline3D.Cleanup(device);
		//cleanup buffers
		//vertexBuffer.Cleanup();
		//indexBuffer.Cleanup();
		//uniformBuffer.Cleanup();

		//cleanup pools
		commandPool.Destroy();
		//descriptorPool.cleanup(); //todo:fix naming convention

		//cleaning ye ole pipes
		//vkDestroyPipeline(device, graphicsPipeline, nullptr);
		//vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		//m_3DShader.Cleanup(device);
		vkDestroyRenderPass(device, renderPass, nullptr);


		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}
		
		vkDestroyDevice(device, nullptr);

		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);
		glfwTerminate();
#pragma region backup
		//backup
		//for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		//	vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		//	vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		//	vkDestroyFence(device, inFlightFences[i], nullptr);
		//}
		////cleanup buffers
		//vertexBuffer.Cleanup();
		//indexBuffer.Cleanup();
		//uniformBuffer.Cleanup();

		////cleanup pools
		//commandPool.Destroy();
		//descriptorPool.cleanup(); //todo:fix naming convention


		////framebuffer cleanup
		//for (auto framebuffer : swapChainFramebuffers) {
		//	vkDestroyFramebuffer(device, framebuffer, nullptr);
		//}

		////cleaning ye ole pipes
		//vkDestroyPipeline(device, graphicsPipeline, nullptr);
		//vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		//m_3DShader.Cleanup(device);
		//vkDestroyRenderPass(device, renderPass, nullptr);

		//for (auto imageView : swapChainImageViews) {
		//	vkDestroyImageView(device, imageView, nullptr);
		//}

		//if (enableValidationLayers) {
		//	DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		//}
		//vkDestroySwapchainKHR(device, swapChain, nullptr);
		//vkDestroyDevice(device, nullptr);

		//vkDestroySurfaceKHR(instance, surface, nullptr);
		//vkDestroyInstance(instance, nullptr);

		//glfwDestroyWindow(window);
		//glfwTerminate();
#pragma endregion
	}



	void createSurface() {
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}

	//GP2Shader m_GradientShader{
	//	"shaders/shader.vert.spv",
	//	"shaders/shader.frag.spv"};

	//GP2Shader3D m_3DShader{ "shaders/shader3D.vert.spv",
	//						"shaders/shader.frag.spv" };

	// Week 01: 
	// Actual window
	// simple fragment + vertex shader creation functions
	// These 5 functions should be refactored into a separate C++ class
	// with the correct internal state.

	GLFWwindow* window;
	void initWindow();



	void drawScene();;
	void draw3DScene(uint32_t imageIndex);

	// Week 02
	// Queue families
	// CommandBuffer concept

	GP2CommandPool commandPool;
	std::vector<GP2CommandBuffer> commandBuffers;
	GP2Mesh mesh2D1{}, mesh2D2{}, mesh2D3{};
	GP2Mesh3D mesh3D1{}, mesh3D2{}, mesh3D3{};
	//GP2VertexBuffer vertexBuffer;
	//GP2IndexBuffer indexBuffer;
	//GP2UniformBuffer uniformBuffer;
	//GP2DescriptorPool descriptorPool;

	std::vector<VkFramebuffer> swapChainFramebuffers;
	//VkPipelineLayout pipelineLayout;
	//VkPipeline graphicsPipeline;
	GP2Pipeline2D m_GraphicsPipeline2D{ "shaders/shader.vert.spv",
										"shaders/shader.frag.spv" };
	GP2Pipeline3D m_GraphicsPipeline3D{ "shaders/shader3D.vert.spv",
										"shaders/shader.frag.spv" };


	VkRenderPass renderPass;

	void createFrameBuffers();
	void createRenderPass();
	void createGraphicsPipeline();
	void createGraphicsPipeline3D();
	void createUniformBuffers();

	uint32_t currentFrame = 0;
	//uint32_t imageIndex = 0;
	//std::vector<GP2UniformBuffer> uniformBuffers;
	//std::vector<VkDeviceMemory> uniformBuffersMemory;
	//std::vector<void*> uniformBuffersMapped;

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);


	void drawFrame(uint32_t imageIndex);
	//void createCommandBuffer();
	//void createCommandPool(); 
	void recordCommandBuffer(uint32_t imageIndex);

	//void CreateVertexBuffer();

	// Week 03

	// Renderpass concept
	// Graphics pipeline
	

	// Week 04
	// Swap chain and image view support

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	std::vector<VkImageView> swapChainImageViews;

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	void createSwapChain();
	void recreateSwapChain();
	void cleanupSwapChain();

	void createImageViews();

	// Week 05 
	// Logical and physical device

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	void createLogicalDevice();

	// Week 06
	// Main initialization

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkDevice device = VK_NULL_HANDLE;
	VkSurfaceKHR surface;


	//INPUT
	void keyEvent(int key, int scancode, int action, int mods);
	void mouseEvent(GLFWwindow* window, int button, int action, int mods);
	void mouseMove(GLFWwindow* window, double xpos, double ypos);
	float m_Radius{};
	glm::vec2 m_Rotation{};
	glm::vec2 m_DragStart{};

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	bool framebufferResized = false; //doing assignment creation bc the tutorial does it that way.

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void setupDebugMessenger();
	std::vector<const char*> getRequiredExtensions();
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	void createInstance();

	void createSyncObjects();
	void drawFrame();

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}
};