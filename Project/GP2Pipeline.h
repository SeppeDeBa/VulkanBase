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

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {}; // Optional
		depthStencil.back = {}; // Optional


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
		pipelineInfo.pDepthStencilState = &depthStencil;
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
		rasterizer.cullMode = VK_CULL_MODE_NONE;
		//rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {}; // Optional
		depthStencil.back = {}; // Optional

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
		pipelineInfo.pDepthStencilState = &depthStencil;
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

	void UpdateUniformBuffers(uint32_t currentImage, const glm::mat4& cameraToWorld)
	{
		for(GP2UniformBuffer& uBuffer : m_UniformBuffers)
		{
			uBuffer.UpdateUniformBuffer(currentImage, cameraToWorld);
		}
	}


private:
	std::vector<GP2Mesh3D> m_Meshes{};
	GP2Shader3D m_Shader;
	std::vector<GP2UniformBuffer> m_UniformBuffers{};
	std::vector<GP2DescriptorPool> m_DescriptorPools{};
};



class GP2PipelineInstance final : public GP2PipelineBase
{
public:
	GP2PipelineInstance(const std::string& vertexShaderFileString, const std::string& fragmentShaderFileString)
		: GP2PipelineBase(vertexShaderFileString, fragmentShaderFileString)
		, m_Shader(vertexShaderFileString, fragmentShaderFileString)
	{

	}
	virtual ~GP2PipelineInstance() override = default;

	virtual void CreateGraphicsPipeline(VkDevice& device, VkRenderPass& renderPass) override
	{
		//init vertexInput instanced
		m_VertexInputBindingDescriptions.emplace_back(Vertex3D::GetBindingDescription());
		for (VkVertexInputAttributeDescription desc : Vertex3D::GetAttributeDescriptions())
		{
			m_VertexInputAttributeDescriptors.emplace_back(desc);
		}

		uint32_t startAttrLoc = static_cast<uint32_t>(m_VertexInputAttributeDescriptors.size());
		m_VertexInputBindingDescriptions.emplace_back(VertexInstance::getBindingDescription());
		for (VkVertexInputAttributeDescription desc : VertexInstance::getAttributeDescriptions(startAttrLoc))
		{
			m_VertexInputAttributeDescriptors.emplace_back(desc);
		};
		//end of vertexInput instanced

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
		rasterizer.cullMode = VK_CULL_MODE_NONE;
		//rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {}; // Optional
		depthStencil.back = {}; // Optional

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

		is.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;//tpdp: already set but doing it just for visual clarity, removed if optimising program
		is.vertexBindingDescriptionCount = static_cast<uint32_t>(m_VertexInputBindingDescriptions.size());
		is.pVertexBindingDescriptions = m_VertexInputBindingDescriptions.data();
		is.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_VertexInputAttributeDescriptors.size());
		is.pVertexAttributeDescriptions = m_VertexInputAttributeDescriptors.data();

		//auto bindingDescription = Vertex3D::GetBindingDescription();
		//auto attributeDescriptions = Vertex3D::GetAttributeDescriptions();
		//is.vertexBindingDescriptionCount = 2; //todo: needs this?
		//is.vertexBindingDescriptionCount = 1;
		//is.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		//is.pVertexBindingDescriptions = &bindingDescription;
		//is.pVertexAttributeDescriptions = attributeDescriptions.data();
		pipelineInfo.pVertexInputState = &is;
		pipelineInfo.pInputAssemblyState = &m_Shader.createInputAssemblyStateInfo();



#pragma region pipelineInfo
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
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


	virtual void Record(std::vector<GP2CommandBuffer>& commBuffers, uint32_t imageIndex, uint32_t currentFrame) override ////change m_ActivateRandomization to activate random InstanceData
	{
		for (int i{}; i < m_Meshes.size(); ++i)
		{
			VkBuffer vertexBuffers[] = { m_VertexBuffers[i].GetBuffer() };
			VkBuffer instanceBuffers[] = {m_InstanceBuffers[i].GetBuffer()};//todo: get could be a problem here with unique
			VkBuffer indexBuffers = { m_IndexBuffers[i].GetBuffer() };
			VkDeviceSize offsets[] = { 0 };
			//bind all buffers
			vkCmdBindVertexBuffers(commBuffers[currentFrame].GetVkCommandBuffer(), 0, 1, vertexBuffers, offsets);
			m_InstanceBuffers[i].UpdateInstanceBuffer(m_InstanceDatas[i]);
			vkCmdBindVertexBuffers(commBuffers[currentFrame].GetVkCommandBuffer(), 1, 1, instanceBuffers, offsets);

			//pushConstant
			// todo: KOEN -> vkCmdPushConstants should be here.
			//vkCmdPushConstants(commBuffers[currentFrame].GetVkCommandBuffer(), m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(), m_VertexConstant

			vkCmdBindIndexBuffer(commBuffers[currentFrame].GetVkCommandBuffer(), indexBuffers, 0, VK_INDEX_TYPE_UINT16);
			vkCmdBindDescriptorSets(commBuffers[currentFrame].GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorPools[i].GetDescriptorSets()[currentFrame], 0, nullptr);
			vkCmdDrawIndexed(commBuffers[currentFrame].GetVkCommandBuffer(), static_cast<uint32_t>(m_Meshes[i].GetIndices().size()), m_Instances, 0, 0, 0);
		
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
		for (GP2InstanceBuffer& iBuffer : m_InstanceBuffers)
		{
			iBuffer.Cleanup();
		}

	}
	virtual void Initialize(const VkDevice& device) override
	{
		m_Shader.initialize(device);
		m_Shader.CreateDescriptorSetLayout(device);
	}

	////change m_ActivateRandomization to activate random InstanceData
	void AddMesh3D(const GP2Mesh3D& meshToAdd, const GP2CommandPool& commPool, VkDevice device, VkPhysicalDevice physDevice, VkQueue queue, const VkImageView& imgView, const VkSampler& texSampler)
	{
		m_Meshes.push_back(meshToAdd);

		//vertex buffer
		m_VertexBuffers.emplace_back();//make new
		m_VertexBuffers.back().Initialize(device, physDevice, commPool.GetCommandPool(), queue);
		m_VertexBuffers.back().CreateBuffer3D(meshToAdd);

		//instance initializing
		//1. instances
		m_InstanceCounts.emplace_back(m_Instances);
		//2. instanceData
		m_InstanceDatas.emplace_back(std::vector<VertexInstance>());//shouldnt need manual VI vect, but placing it to show that its a vector of the vertices

		m_InstanceDatas.back().resize(m_Instances);
		//3. instanceBuffer
		m_InstanceBuffers.emplace_back();
		m_InstanceBuffers.back().Initialize(device, physDevice, commPool.GetCommandPool(), queue);
		for (int i{}; i < m_Instances; ++i)
		{
			setInstanceData(m_MeshCount, i, glm::vec3{ i * m_XOffsetPerInstance,2.f,1.f }, glm::vec2(static_cast<float>(rand()% m_TextureCols), static_cast<float>(rand() % m_TextureRows)));
		}

		m_InstanceBuffers.back().CreateInstanceBuffer(m_InstanceDatas.back());
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


		++m_MeshCount;
	};

	const std::vector<GP2UniformBuffer>& GetUniformBuffers() { return m_UniformBuffers; };
	const std::vector<GP2DescriptorPool>& GetDescriptorPools() { return m_DescriptorPools; };

	void UpdateUniformBuffers(uint32_t currentImage, const glm::mat4& cameraToWorld)
	{
		for (GP2UniformBuffer& uBuffer : m_UniformBuffers)
		{
			uBuffer.UpdateUniformBuffer(currentImage, cameraToWorld);
		}
	}


private:
	std::vector<GP2Mesh3D> m_Meshes{};

	void setInstanceData(int meshNr, uint32_t instanceId, glm::vec3 t, glm::vec2 tc) //change m_ActivateRandomization to activate random InstanceData
	{
		if (instanceId < m_InstanceCounts[meshNr])
		{
			if(m_ActivateRandomization)
			{
				glm::vec3 randomOffset = t;
				randomOffset += (rand() % (m_MaxTranslation + m_MinTranslation)) + 1.f - m_MinTranslation;
				m_InstanceDatas[meshNr][instanceId].modelTransform = glm::translate(glm::mat4(1.0), randomOffset);
				m_InstanceDatas[meshNr][instanceId].modelTransform = glm::rotate(m_InstanceDatas[meshNr][instanceId].modelTransform,
					glm::radians(static_cast<float>(rand() % m_MaxRandRotation)), glm::vec3(0.f,0.f,1.f));
				m_InstanceDatas[meshNr][instanceId].modelTransform = glm::scale(m_InstanceDatas[meshNr][instanceId].modelTransform,
					glm::vec3(static_cast<float>(rand() % m_MaxRandScale)+1.f));//adding 1 so its never scale 0
			}
			else
			{
				m_InstanceDatas[meshNr][instanceId].modelTransform = glm::translate(glm::mat4(1.0), t);
				
			}


			m_InstanceDatas[meshNr][instanceId].texCoord = tc;
		}
	}
	//instanced stuff
	int m_MeshCount{0};
	//instanced settings //IMPORTANT NOTE. Using non seeded rands and non decimals, just for ease of use and proof that it works as there is another deadline tomorrow :)
	bool m_ActivateRandomization{ false };
	const uint16_t m_Instances{ 20 };
	const float m_XOffsetPerInstance{ 1.f };
	const int m_TextureCols{ 16 };
	const int m_TextureRows{ 16 };
	const int m_MaxRandScale{2};
	const int m_MaxRandRotation{360};
	const int m_MinTranslation{ 3 };
	const int m_MaxTranslation{ 5 };
	const glm::vec2 m_MaxRandTranslation{3.f};


	std::vector<uint16_t> m_InstanceCounts{ m_Instances };
	std::vector<std::vector<VertexInstance>> m_InstanceDatas{};
	std::vector<GP2InstanceBuffer> m_InstanceBuffers{ };




	std::vector<VkVertexInputBindingDescription> m_VertexInputBindingDescriptions{};
	std::vector<VkVertexInputAttributeDescription> m_VertexInputAttributeDescriptors{};


	GP2Shader3D m_Shader;
	std::vector<GP2UniformBuffer> m_UniformBuffers{};
	std::vector<GP2DescriptorPool> m_DescriptorPools{};
};


