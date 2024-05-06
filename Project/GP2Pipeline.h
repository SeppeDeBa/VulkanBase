#pragma once
#include "vulkanbase/VulkanBase.h"
#include "GP2CommandBuffer.h"
#include "GP2DataBuffer.h"
#include <string>
class GP2PipelineBase
{
	
public:
	GP2PipelineBase(const std::string& vertexShaderFileString, const std::string& fragmentShaderFileString)
		: m_VertexShaderFile{ vertexShaderFileString }
		, m_FragmentShaderFile{ fragmentShaderFileString }
	{

	}
	virtual ~GP2PipelineBase() = default;
	virtual void CreateGraphicsPipeline(VkDevice& device, VkRenderPass& renderPass) = 0;

	virtual void Initialize(const VkDevice& device) = 0;
	virtual void Record(std::vector<GP2CommandBuffer>& commBuffers, uint32_t imageIndex, uint32_t currentFrame) = 0;
	virtual void Cleanup(VkDevice& device)
	{
		for (GP2VertexBuffer& vBuffer : m_VertexBuffers)
		{
			vBuffer.Cleanup();
		}
		for (GP2IndexBuffer& iBuffer : m_IndexBuffers)
		{
			iBuffer.Cleanup();
		}

		vkDestroyPipeline(device, m_Pipeline, nullptr);
		vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);
	}

	const std::string& getVertexShaderFile() const { return m_VertexShaderFile; };
	const std::string& getFragmentShaderFile() const { return m_FragmentShaderFile; };

	const std::vector<GP2VertexBuffer>& GetVertexBuffers() { return m_VertexBuffers; };
	const std::vector<GP2IndexBuffer>& GetIndexBuffers() { return m_IndexBuffers; };
	VkPipeline& GetPipeline() { return m_Pipeline;};
	
protected:
	std::vector<GP2VertexBuffer> m_VertexBuffers{};
	std::vector<GP2IndexBuffer> m_IndexBuffers{};
	//std::vector<GP2UniformBuffer> m_UniformBuffers{};
	//std::vector<GP2DescriptorPool> m_DescriptorPools{};
	VkPipelineLayout m_PipelineLayout{};
	VkPipeline m_Pipeline{};
	
private:
	//int m_amtOfMeshes{};
	const std::string m_VertexShaderFile;
	const std::string m_FragmentShaderFile;
};


class GP2Pipeline2D final : public GP2PipelineBase
{
public:
	GP2Pipeline2D(const std::string& vertexShaderFileString, const std::string& fragmentShaderFileString)
		: GP2PipelineBase(vertexShaderFileString, fragmentShaderFileString)
		, m_Shader(vertexShaderFileString, fragmentShaderFileString)
	{
		
	}
	virtual ~GP2Pipeline2D() override = default;
	virtual void CreateGraphicsPipeline(VkDevice& device, VkRenderPass& renderPass) override
	{
		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pushConstantRangeCount = 0;

		if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}

		VkGraphicsPipelineCreateInfo pipelineInfo{};

		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = m_Shader.getShaderStages().data();//same
		VkPipelineVertexInputStateCreateInfo is = m_Shader.createVertexInputStateInfo();


		auto bindingDescription = Vertex::GetBindingDescription();
		auto attributeDescription = Vertex::GetAttributeDescriptions();
		is.vertexBindingDescriptionCount = 1;
		is.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
		is.pVertexBindingDescriptions = &bindingDescription;
		is.pVertexAttributeDescriptions = attributeDescription.data();

		pipelineInfo.pVertexInputState = &is;
		pipelineInfo.pInputAssemblyState = &m_Shader.createInputAssemblyStateInfo();

#pragma region pipelineInfo
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = m_PipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
#pragma endregion 
		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline!");
		}

		m_Shader.destroyShaderModules(device);
	}
	virtual void Record(std::vector<GP2CommandBuffer>& commBuffers, uint32_t imageIndex, uint32_t currentFrame) override
	{
		for (int i{}; i < m_Meshes.size(); ++i)
		{
			VkBuffer vertexBuffers[] = { m_VertexBuffers[i].GetBuffer()};
			VkBuffer indexBuffers = { m_IndexBuffers[i].GetBuffer()};
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commBuffers[currentFrame].GetVkCommandBuffer(), 0, 1, vertexBuffers, offsets);


			vkCmdBindIndexBuffer(commBuffers[currentFrame].GetVkCommandBuffer(), indexBuffers, 0, VK_INDEX_TYPE_UINT16);

			//VkBuffer vertexBuffers[] = { vertexBuffer };
			//VkDeviceSize offsets[] = { 0 };

			vkCmdDrawIndexed(commBuffers[currentFrame].GetVkCommandBuffer(), static_cast<uint32_t>(m_Meshes[i].GetIndices().size()), 1, 0, 0, 0);
		}
		//vkCmdDraw(commandBuffer.getVkCommandBuffer(), 6, 1, 0, 0);
	}
	virtual void Cleanup(VkDevice& device) override
	{
		GP2PipelineBase::Cleanup(device);
		
	}
	virtual void Initialize(const VkDevice& device) override
	{
		m_Shader.initialize(device);
	}

	void AddMesh2D(const GP2Mesh& meshToAdd, const GP2CommandPool& commPool ,VkDevice device, VkPhysicalDevice physDevice, VkQueue queue)
	{
		m_Meshes.push_back(meshToAdd);

		//vertex buffer
		m_VertexBuffers.emplace_back();//make new
		m_VertexBuffers.back().Initialize(device, physDevice, commPool.GetCommandPool(), queue);
		m_VertexBuffers.back().CreateBuffer(meshToAdd);

		//Index Buffer
		m_IndexBuffers.emplace_back();
		m_IndexBuffers.back().Initialize(device, physDevice, commPool.GetCommandPool(), queue);
		m_IndexBuffers.back().CreateBuffer(meshToAdd);
	};
private:
	std::vector<GP2Mesh> m_Meshes{};
	GP2Shader m_Shader;



};

class GP2Pipeline3D final : public GP2PipelineBase
{
public:
	GP2Pipeline3D(const std::string& vertexShaderFileString, const std::string& fragmentShaderFileString)
		: GP2PipelineBase(vertexShaderFileString, fragmentShaderFileString)
		, m_Shader(vertexShaderFileString, fragmentShaderFileString)
	{
		
	}
	virtual ~GP2Pipeline3D() override = default;

	virtual void CreateGraphicsPipeline(VkDevice& device, VkRenderPass& renderPass) override
	{
		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &(m_Shader.GetDescriptorSetLayout());

		if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}

		VkGraphicsPipelineCreateInfo pipelineInfo{};

		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = m_Shader.getShaderStages().data();
		VkPipelineVertexInputStateCreateInfo is = m_Shader.createVertexInputStateInfo();

		auto bindingDescription = Vertex3D::GetBindingDescription();
		auto attributeDescriptions = Vertex3D::GetAttributeDescriptions();
		is.vertexBindingDescriptionCount = 1;
		is.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		is.pVertexBindingDescriptions = &bindingDescription;
		is.pVertexAttributeDescriptions = attributeDescriptions.data();
		pipelineInfo.pVertexInputState = &is;
		pipelineInfo.pInputAssemblyState = &m_Shader.createInputAssemblyStateInfo();

#pragma region pipelineInfo
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = m_PipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		
#pragma endregion
		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline!");
		}
		m_Shader.destroyShaderModules(device);
	}


	virtual void Record(std::vector<GP2CommandBuffer>& commBuffers, uint32_t imageIndex, uint32_t currentFrame) override
	{
		for (int i{}; i < m_Meshes.size(); ++i)
		{
			VkBuffer vertexBuffers[] = { m_VertexBuffers[i].GetBuffer()};
			VkBuffer indexBuffers = { m_IndexBuffers[i].GetBuffer()};
			VkDeviceSize offsets[] = { 0 };
			//bind all buffers
			vkCmdBindVertexBuffers(commBuffers[currentFrame].GetVkCommandBuffer(), 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(commBuffers[currentFrame].GetVkCommandBuffer(), indexBuffers, 0, VK_INDEX_TYPE_UINT16);
			vkCmdBindDescriptorSets(commBuffers[currentFrame].GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorPools[i].GetDescriptorSets()[currentFrame], 0, nullptr);
			vkCmdDrawIndexed(commBuffers[currentFrame].GetVkCommandBuffer(), static_cast<uint32_t>(m_Meshes[i].GetIndices().size()), 1, 0, 0, 0);
		}
		//vkCmdDraw(commandBuffer.getVkCommandBuffer(), 6, 1, 0, 0);
	}
	virtual void Cleanup(VkDevice& device) override
	{
		//std::vector<GP2UniformBuffer> m_UniformBuffers{};
//std::vector<GP2DescriptorPool> m_DescriptorPools{};
		GP2PipelineBase::Cleanup(device);

		for (GP2UniformBuffer& uBuffer : m_UniformBuffers)
		{
			uBuffer.Cleanup();
		}
		for (GP2DescriptorPool& dPools : m_DescriptorPools)
		{
			dPools.cleanup();
		}
		m_Shader.Cleanup(device);
	
	}
	virtual void Initialize(const VkDevice& device) override
	{
		m_Shader.initialize(device);
		m_Shader.CreateDescriptorSetLayout(device);
	}

	void AddMesh3D(const GP2Mesh3D& meshToAdd, const GP2CommandPool& commPool, VkDevice device, VkPhysicalDevice physDevice, VkQueue queue, const VkImageView& imgView, const VkSampler& texSampler)
	{
		m_Meshes.push_back(meshToAdd);

		//vertex buffer
		m_VertexBuffers.emplace_back();//make new
		m_VertexBuffers.back().Initialize(device, physDevice, commPool.GetCommandPool(), queue);
		m_VertexBuffers.back().CreateBuffer3D(meshToAdd);

		//Index Buffer
		m_IndexBuffers.emplace_back();
		m_IndexBuffers.back().Initialize(device, physDevice, commPool.GetCommandPool(), queue);
		m_IndexBuffers.back().CreateBuffer3D(meshToAdd);

		////UniformBuffer
		m_UniformBuffers.emplace_back();
		m_UniformBuffers.back().Initialize(device, physDevice, commPool.GetCommandPool(), queue);
		m_UniformBuffers.back().CreateBuffer3D(meshToAdd);

		////descriptorPool
		m_DescriptorPools.emplace_back();
		m_DescriptorPools.back().createDescriptorPool(device);
		m_DescriptorPools.back().createDescriptorSets(m_Shader.GetDescriptorSetLayout(), m_UniformBuffers.back(), imgView, texSampler);
	};

	const std::vector<GP2UniformBuffer>& GetUniformBuffers() { return m_UniformBuffers; };
	const std::vector<GP2DescriptorPool>& GetDescriptorPools() { return m_DescriptorPools; };

	void UpdateUniformBuffers(uint32_t currentImage)
	{
		for(GP2UniformBuffer& uBuffer : m_UniformBuffers)
		{
			uBuffer.UpdateUniformBuffer(currentImage);
		}
	}


private:
	std::vector<GP2Mesh3D> m_Meshes{};
	GP2Shader3D m_Shader;
	std::vector<GP2UniformBuffer> m_UniformBuffers{};
	std::vector<GP2DescriptorPool> m_DescriptorPools{};
};