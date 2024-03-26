#include "GP2DataBuffer.h"
#include <stdexcept>
#include <GP2Mesh.h>

GP2DataBuffer::GP2DataBuffer(VkDevice device, VkPhysicalDevice physDevice, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceSize size)
    :m_VkDevice(device)
    , m_Size(size)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_VkBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, m_VkBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(physDevice, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &m_VkBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device, m_VkBuffer, m_VkBufferMemory, 0);
}

GP2DataBuffer::~GP2DataBuffer()
{
    Destroy();
}

void GP2DataBuffer::Destroy()
{
    vkDestroyBuffer(m_VkDevice, m_VkBuffer, nullptr);
    vkFreeMemory(m_VkDevice, m_VkBufferMemory, nullptr);
}