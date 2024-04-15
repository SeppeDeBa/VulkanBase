#pragma once
#ifndef MAX_FRAMES_IN_FLIGHT
#define MAX_FRAMES_IN_FLIGHT 2
#endif // !MAX_FRAMES_IN_FLIGHT

#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <chrono>
#include "GP2Mesh.h"

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

class GP2DataBuffer
{
public:
	GP2DataBuffer(VkDevice device, VkPhysicalDevice physDevice, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceSize size);
	~GP2DataBuffer();

	void Destroy();

	const VkBuffer& GetBuffer() const { return m_VkBuffer; }
	const VkDeviceMemory& GetBufferMemory() const { return m_VkBufferMemory; }
	const VkDeviceSize& GetSize() const { return m_Size; }

private:
	VkDevice m_VkDevice;
	VkDeviceSize m_Size;
	VkBuffer m_VkBuffer;
	VkDeviceMemory m_VkBufferMemory;

	uint32_t FindMemoryType(VkPhysicalDevice physDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}
		return 0;
	}
};

class GP2BufferBase
{
public:
	virtual void Cleanup()
	{
		delete m_BufferInfo;
	}

	virtual const VkBuffer& GetBuffer() const { return m_BufferInfo->GetBuffer(); }
	virtual const VkDeviceMemory& GetBufferMemory() const { return m_BufferInfo->GetBufferMemory(); }

	virtual void Initialize(const VkDevice& device, const VkPhysicalDevice& physDevice, const VkCommandPool& commandPool, const VkQueue& graphicsQueue)
	{
		m_Device = device;
		m_PhysicalDevice = physDevice;
		m_CommandPool = commandPool;
		m_GraphicsQueue = graphicsQueue;
	}

	virtual void CreateBuffer(const GP2Mesh& mesh) = 0;

protected:
	VkDevice m_Device;
	VkPhysicalDevice m_PhysicalDevice;
	VkCommandPool m_CommandPool;
	VkQueue m_GraphicsQueue;

	GP2DataBuffer* m_BufferInfo;

	virtual void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_CommandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(m_Device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_GraphicsQueue);

		vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &commandBuffer);
	}
};

class GP2VertexBuffer : public GP2BufferBase
{
public:
	virtual void CreateBuffer(const GP2Mesh& mesh) override
	{
		VkDeviceSize bufferSize = sizeof(Vertex) * mesh.GetVertices().size();

		GP2DataBuffer stagingBuffer{ m_Device
									, m_PhysicalDevice
									, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
									, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
									, bufferSize };

		void* data;
		vkMapMemory(m_Device, stagingBuffer.GetBufferMemory(), 0, bufferSize, 0, &data);
		memcpy(data, mesh.GetVertices().data(), (size_t)bufferSize);
		vkUnmapMemory(m_Device, stagingBuffer.GetBufferMemory());

		m_BufferInfo = new GP2DataBuffer(m_Device
			, m_PhysicalDevice
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, bufferSize);

		CopyBuffer(stagingBuffer.GetBuffer(), m_BufferInfo->GetBuffer(), bufferSize);
		/*vkDestroyBuffer(m_Device, stagingBuffer.GetBuffer(), nullptr);
		vkFreeMemory(m_Device, stagingBuffer.GetBufferMemory(), nullptr);*/
	}
};

class GP2IndexBuffer : public GP2BufferBase
{
public:
	virtual void CreateBuffer(const GP2Mesh& mesh) override
	{
		VkDeviceSize bufferSize = sizeof(mesh.GetIndices()[0]) * mesh.GetIndices().size();

		GP2DataBuffer stagingBuffer{ m_Device
									, m_PhysicalDevice
									, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
									, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
									, bufferSize };

		void* data;
		vkMapMemory(m_Device, stagingBuffer.GetBufferMemory(), 0, bufferSize, 0, &data);
		memcpy(data, mesh.GetIndices().data(), (size_t)bufferSize);
		vkUnmapMemory(m_Device, stagingBuffer.GetBufferMemory());

		m_BufferInfo = new GP2DataBuffer(m_Device
			, m_PhysicalDevice
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, bufferSize);

		CopyBuffer(stagingBuffer.GetBuffer(), m_BufferInfo->GetBuffer(), bufferSize);
		/*vkDestroyBuffer(m_Device, stagingBuffer.GetBuffer(), nullptr);
		vkFreeMemory(m_Device, stagingBuffer.GetBufferMemory(), nullptr);*/
	}
};

class GP2UniformBuffer : public GP2BufferBase
{
public:
	virtual void Cleanup() override
	{
		for (auto uniformBuffer : m_UniformBufferInfos)
		{
			delete uniformBuffer;
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_UniformBufferInfos[i]->Destroy();
		}
		//is this needed?
		//for (auto uniformBufferMapped : uniformBuffersMapped)
		//{
		//	delete uniformBufferMapped;
		//}
	}

	virtual void CreateBuffer(const GP2Mesh&) override
	{
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);

		m_UniformBufferInfos.resize(MAX_FRAMES_IN_FLIGHT);
		m_UniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);
		m_UniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			m_UniformBufferInfos[i] = new GP2DataBuffer{ m_Device
														, m_PhysicalDevice
														, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
														, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
														, bufferSize };

			
			vkMapMemory(m_Device, m_UniformBufferInfos[i]->GetBufferMemory(), 0, bufferSize, 0,
				&m_UniformBuffersMapped[i]);
			//uniformBuffersMapped[i] = static_cast<VkDeviceMemory*>(pData);
		}
	}

	void UpdateUniformBuffer(uint32_t currentImage)
	{
		static auto startTime = std::chrono::high_resolution_clock::now();
	
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	
		UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), WIDTH / static_cast<float>(HEIGHT), 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;
	
		memcpy(m_UniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
	}

	const std::vector<GP2DataBuffer*>& GetUniformBufferInfos() const { return m_UniformBufferInfos; };
private:
	std::vector<GP2DataBuffer*> m_UniformBufferInfos{};
	std::vector<void*> m_UniformBuffersMapped{};
	std::vector<VkDeviceMemory> m_UniformBuffersMemory{};
};