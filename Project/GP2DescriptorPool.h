#pragma once
#include "GP2DataBuffer.h"
#include "vulkan/vulkan_core.h"

//Reference: https://vulkan-tutorial.com/en/Uniform_buffers/Descriptor_pool_and_sets
class GP2DescriptorPool //TODO: refactor with correct capitalisation!!!
{
public:
	GP2DescriptorPool() //based on commandPool
		: m_Device(VK_NULL_HANDLE)
		, m_DescriptorPool(VK_NULL_HANDLE)
	{};
	~GP2DescriptorPool() = default;

	void createDescriptorPool(const VkDevice& device)
	{
		m_Device = device;
		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
		m_Created = true;

		//m_Device = device;
		////size
		//VkDescriptorPoolSize poolSize{};
		//poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		//poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		////info
		//VkDescriptorPoolCreateInfo poolInfo{};
		//poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		//poolInfo.poolSizeCount = 1;
		//poolInfo.pPoolSizes = &poolSize;

		////max number of descriptor sets
		//poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		////check what can be freed, default flag value can be 0
		////VkDescriptorPool  descriptorPool;

		//if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) { //todo problem here
		//	throw std::runtime_error("failed to create descriptor pool!");
		//}
		//m_Created = true;
	}

	void createDescriptorSets(const VkDescriptorSetLayout& descriptorSetLayout, const GP2UniformBuffer& uniformBuffers, const VkImageView& texImgView, const VkSampler texSampler)
	{
		//check order of operations
		if(!m_Created)
		{
			throw std::runtime_error("Creating descriptors with no descriptorPool initialized");
		}
		//set allocation, based on descriptor layout
		std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_DescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

		//allocate descriptor set handles
		m_DescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(m_Device, &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		//populate every descriptor
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			//fill info
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffers.GetUniformBufferInfos()[i]->GetBuffer();//todo: ask is get buffer needed?
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = texImgView;
			imageInfo.sampler = texSampler;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = m_DescriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = m_DescriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(m_Device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

			////overwriting whole buffer
			//VkWriteDescriptorSet descriptorWrite{};
			//descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			//descriptorWrite.dstSet = m_DescriptorSets[i];
			//descriptorWrite.dstBinding = 0;
			//descriptorWrite.dstArrayElement = 0;

			//descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			//descriptorWrite.descriptorCount = 1;

			////specify how many array elements
			//descriptorWrite.pBufferInfo = &bufferInfo;
			//descriptorWrite.pImageInfo = nullptr; // Optional
			//descriptorWrite.pTexelBufferView = nullptr; // Optional

			//needs ref
			//vkUpdateDescriptorSets(m_Device, 1, &descriptorWrite, 0, nullptr);
		}


	} 

	void cleanup()
	{
		vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);

		//todo: how to destroy then? and needed?
		//vkDestroyDescriptorSetLayout(m_Device, m_Device, nullptr);
	}


	const VkDescriptorPool& GetDescriptorPool() const { return m_DescriptorPool; };
	const std::vector<VkDescriptorSet>& GetDescriptorSets() const { return m_DescriptorSets; };


private:
	bool m_Created{false};

	VkDevice m_Device;
	VkDescriptorPool m_DescriptorPool;
	std::vector<VkDescriptorSet> m_DescriptorSets;

};