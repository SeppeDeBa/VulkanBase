#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>

class GP2Shader
{
public:
	GP2Shader(const std::string& vertexShaderFile,
		const std::string& fragmentShaderFile
	) :m_VertexShaderFile(vertexShaderFile)
		, m_FragmentShaderFile(fragmentShaderFile)
	{

	}
	virtual ~GP2Shader() = default;
	GP2Shader(const GP2Shader&) = delete;
	GP2Shader& operator= (const GP2Shader&) = delete;
	GP2Shader(const GP2Shader&&) = delete;
	GP2Shader& operator= (const GP2Shader&&) = delete;



	virtual const std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages() { return m_ShaderStages; };


	void initialize(const VkDevice& vkDevice);
	void destroyShaderModules(const VkDevice& vkDevice);

	virtual VkPipelineVertexInputStateCreateInfo createVertexInputStateInfo();
	virtual VkPipelineInputAssemblyStateCreateInfo createInputAssemblyStateInfo();
protected:
	virtual VkPipelineShaderStageCreateInfo createFragmentShaderInfo(const VkDevice& vkDevice);
	virtual VkPipelineShaderStageCreateInfo createVertexShaderInfo(const VkDevice& vkDevice);
	virtual VkShaderModule createShaderModule(const VkDevice& vkDevice, const std::vector<char>& code);

	std::string m_VertexShaderFile{};
	std::string m_FragmentShaderFile{};

	std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages{};

};
 
class GP2Shader3D : public GP2Shader  
{
public:
	GP2Shader3D(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
		:GP2Shader(vertexShaderFile, fragmentShaderFile)
	{
	}

	void CreateDescriptorSetLayout(const VkDevice& vkDevice);
	void Cleanup(const VkDevice& vkDevice);
	const VkDescriptorSetLayout& GetDescriptorSetLayout() const { return m_DescriptorSetLayout; };
private:
	VkDescriptorSetLayout m_DescriptorSetLayout{ nullptr };
};