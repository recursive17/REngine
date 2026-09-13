#include "../REngine.hpp"
using namespace re;


//add logging

void Buffer::create(Device* device, VkDeviceSize deviceSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    device_ = device;
    size_ = deviceSize;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = deviceSize;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device_->get(), &bufferInfo, nullptr, &buffer_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device_->get(), buffer_, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device_->get(), &allocInfo, nullptr, &memory_) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory");
    }

    vkBindBufferMemory(device_->get(), buffer_, memory_, 0);
}

void Buffer::copyFrom(Buffer* srcBuffer, CommandContext* cmdContext, VkDeviceSize size) {
    VkCommandBuffer cmd = cmdContext->beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(cmd, srcBuffer->get(), buffer_, 1, &copyRegion);

    cmdContext->endSingleTimeCommands(cmd);
}

void Buffer::map(void** dataOut) {
    vkMapMemory(device_->get(), memory_, 0, size_, 0, dataOut);
}

void Buffer::unmap() {
    vkUnmapMemory(device_->get(), memory_);
}

VkBuffer Buffer::get() const {
    return buffer_;
}
VkDeviceMemory Buffer::getMemory() const {
    return memory_;
}

Buffer::~Buffer() {
    if (buffer_ != VK_NULL_HANDLE) {
        vkDestroyBuffer(device_->get(), buffer_, nullptr);
    }
    if (memory_ != VK_NULL_HANDLE) {
        vkFreeMemory(device_->get(), memory_, nullptr);
    }
}

uint32_t Buffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device_->getPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type");
}