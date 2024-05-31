#pragma once
#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>
#include <array>
#include <vector>
#include <stdexcept>
#include "vulkanbase/VulkanUtil.h"
#include <memory>
#include <tiny_obj_loader.h>


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

struct Vertex3D {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;
	//glm::vec3 normal;

	static VkVertexInputBindingDescription GetBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex3D);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		//OPTIONS:
		//VK_VERTEX_INPUT_RATE_VERTEX: Move to the next data entry after each vertex
		//VK_VERTEX_INPUT_RATE_INSTANCE : Move to the next data entry after each instance
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions() //https://vulkan-tutorial.com/Depth_buffering
	{
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex3D, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex3D, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex3D, texCoord);

		return attributeDescriptions;
	}
};

struct VertexInstance
{
	glm::mat4 modelTransform;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 1; //should be non hardcoded in bigger render/game engines. could be managed
		bindingDescription.stride = sizeof(VertexInstance);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
		//OPTIONS:
		//VK_VERTEX_INPUT_RATE_VERTEX: Move to the next data entry after each vertex
		//VK_VERTEX_INPUT_RATE_INSTANCE : Move to the next data entry after each instance
		return bindingDescription;
	}
	static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions(uint32_t location)
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{5};

		uint32_t binding = 1;
		for (int i = 0; i < 4; i++) 
		{
			attributeDescriptions[i].binding = binding;
			attributeDescriptions[i].location = location + i;
			attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			attributeDescriptions[i].offset = sizeof(float) * 4 * i;
		}

		attributeDescriptions[4].binding = binding;
		attributeDescriptions[4].location = location + 4;
		attributeDescriptions[4].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[4].offset = offsetof(VertexInstance, texCoord);

		return attributeDescriptions;
	}

	//static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions(uint32_t location)
	//{
	//	std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

	//	attributeDescriptions[0].binding = 0;
	//	attributeDescriptions[0].location = location;
	//	attributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	//	attributeDescriptions[0].offset = offsetof(VertexInstance, modelTransform);

	//	attributeDescriptions[1].binding = 0;
	//	attributeDescriptions[1].location = location + 1;
	//	attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
	//	attributeDescriptions[1].offset = offsetof(VertexInstance, texCoord);



	//	return attributeDescriptions;
	//}
};

class GP2Mesh3D
{
public:

	void SetIndices(const std::vector<uint16_t>& input)
	{
		indices = input;
	}
	void SetVertices(const std::vector<Vertex3D>& input)
	{
		vertices = input;
	}

	void BuildMeshFromOBJ(const std::string& filename)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str())) {
			throw std::runtime_error(warn + err);
		}

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex3D vertex{};

				vertex.pos = { attrib.vertices[3 * index.vertex_index + 0],
								attrib.vertices[3 * index.vertex_index + 1],
								attrib.vertices[3 * index.vertex_index + 2] };

				vertex.texCoord = {attrib.texcoords[2 * index.texcoord_index + 0],
									 1-attrib.texcoords[2 * index.texcoord_index + 1] };

				vertex.color = { 1.f, 1.f, 1.f };

				vertices.push_back(vertex);
				indices.push_back(static_cast<uint32_t>(indices.size()));
			}
		}


		/*if (!ParseOBJ(filename, vertices, indices))
		{
			throw std::runtime_error("failed to read file: " + filename);


		}*/
	}

	static bool ParseOBJ(const std::string& filename, std::vector<Vertex3D>& positions, std::vector<uint16_t>& indices)
	{
		std::ifstream file(filename);
		if (!file)
			return false;

		std::string sCommand;
		// start a while iteration ending when the end of file is reached (ios::eof)
		while (!file.eof())
		{
			//read the first word of the string, use the >> operator (istream::operator>>) 
			file >> sCommand;
			//use conditional statements to process the different commands	
			if (sCommand == "#")
			{
				// Ignore Comment
			}
			
			else if (sCommand == "v")
			{
				//Vertex
				float x, y, z;
				file >> x >> y >> z;
				Vertex3D vertexOutput{};
				glm::vec3 vertexPos{};
				vertexOutput.pos = { x,y,z };
				vertexOutput.color = { 1,1,1 };
				positions.push_back(vertexOutput);
			}
			else if (sCommand == "f")
			{
				float i0, i1, i2;
				file >> i0 >> i1 >> i2;

				indices.push_back((uint32_t)i0 - 1);
				indices.push_back((uint32_t)i1 - 1);
				indices.push_back((uint32_t)i2 - 1);
			}
			//read till end of line and ignore all remaining chars
			file.ignore(1000, '\n');

			if (file.eof())
				break;
		}
		return true;
	}

	const std::vector<Vertex3D>& GetVertices() const { return vertices; }
	const std::vector<uint16_t>& GetIndices() const { return indices; }



private:
	std::vector<Vertex3D> vertices = {


	};
	std::vector<uint16_t> indices = {
	};;
	//vulkanbuffer vertices
	//vulkanbuffer indices

};


class GP2Mesh
{
public:

	void SetIndices(const std::vector<uint16_t>& input)
	{
		indices = input;
	}
	void SetVertices(const std::vector<Vertex>& input)
	{
		vertices = input;
	}

	const std::vector<Vertex>& GetVertices() const { return vertices; }
	const std::vector<uint16_t>& GetIndices() const{ return indices;  }
private:
	std::vector<Vertex> vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};
	std::vector<uint16_t> indices = { 0, 1, 2, 2, 3, 0 };

	uint16_t m_InstanceCount{ 1 };
	std::vector<VertexInstance> m_InstanceData;

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
//		vkMapMemory(m_Device, m_VertexBufferMemory, 0, bufferInfo.
// , 0, &data);
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

