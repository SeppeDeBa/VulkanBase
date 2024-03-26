#pragma once
#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>
#include <array>
#include <vector>
#include <stdexcept>
#include "vulkanbase/VulkanUtil.h"
#include <memory>
//if changing, change where it's set for CMAKE!!!
struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;

	static VkVertexInputBindingDescription GetBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		//OPTIONS:
		//VK_VERTEX_INPUT_RATE_VERTEX: Move to the next data entry after each vertex
		//VK_VERTEX_INPUT_RATE_INSTANCE : Move to the next data entry after each instance
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions() //https://vulkan-tutorial.com/Vertex_buffers/Vertex_input_description
	{
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}
};

class GP2Mesh
{
public:
	const std::vector<Vertex>& GetVertices() const { return vertices; }
	const std::vector<uint16_t>& GetIndices() const{ return indices;  }
private:
	const std::vector<Vertex> vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};
	const std::vector<uint16_t> indices = { 0, 1, 2, 2, 3, 0 };
	//vulkanbuffer vertices
	//vulkanbuffer indices

};

//class GP2VertexBuffer
//{
//public:
//	void Cleanup()
//	{
//		vkDestroyBuffer(m_Device, m_VertexBuffer, nullptr);
//		vkFreeMemory(m_Device, m_VertexBufferMemory, nullptr);
//	}
//
//	const VkBuffer& GetVertexBuffer() const { return m_VertexBuffer; }
//	const VkDeviceMemory& GetVertexBufferMemory() const { return m_VertexBufferMemory; }
//
//	void Initialize(const VkDevice& device, const VkPhysicalDevice& physDevice)
//	{
//		m_Device = device;
//		m_PhysicalDevice = physDevice;
//	}
//
//	void CreateVertexBuffer(const GP2Mesh& mesh)
//	{
//		VkBufferCreateInfo bufferInfo{};
//		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//		bufferInfo.size = sizeof(mesh.GetVertices()[0]) * mesh.GetVertices().size();
//		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
//		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//		if (vkCreateBuffer(m_Device, &bufferInfo, nullptr, &m_VertexBuffer) != VK_SUCCESS)
//		{
//			throw std::runtime_error("failed to create vertex buffer!");
//		}
//
//		VkMemoryRequirements memRequirements;
//		vkGetBufferMemoryRequirements(m_Device, m_VertexBuffer, &memRequirements); // Get memory requirements
//
//		VkMemoryAllocateInfo allocInfo{};
//		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//		allocInfo.allocationSize = memRequirements.size;
//		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
//
//		if (vkAllocateMemory(m_Device, &allocInfo, nullptr, &m_VertexBufferMemory) != VK_SUCCESS)
//		{
//			throw std::runtime_error("failed to allocate vertex buffer memory!");
//		}
//
//		vkBindBufferMemory(m_Device, m_VertexBuffer, m_VertexBufferMemory, 0);
//
//		void* data;
//		vkMapMemory(m_Device, m_VertexBufferMemory, 0, bufferInfo.size, 0, &data);
//		memcpy(data, mesh.GetVertices().data(), (size_t)bufferInfo.size);
//		vkUnmapMemory(m_Device, m_VertexBufferMemory);
//	}
//
//private:
//	VkBuffer m_VertexBuffer;
//	VkDevice m_Device;
//	VkPhysicalDevice m_PhysicalDevice;
//	VkDeviceMemory m_VertexBufferMemory;
//
//	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
//	{
//		VkPhysicalDeviceMemoryProperties memProperties;
//		vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);
//
//		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
//		{
//			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
//			{
//				return i;
//			}
//		}
//	}
//
//};

