#include "../rengine.hpp"
using namespace re;


//add logging

void Buffer::create(Device* device, VkDeviceSize deviceSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage) {
    device_ = device;
    size_ = deviceSize;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = deviceSize;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = memoryUsage;
        allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;


    if (vmaCreateBuffer(device_->getAllocator(), &bufferInfo, &allocInfo, &buffer_, &allocation_, nullptr) != VK_SUCCESS) {
        re::ReLog::error(re::Prefix::VULKAN, std::runtime_error("failed to crate vma buffer"));
    }
}

void Buffer::copyFrom(Buffer* srcBuffer, CommandContext* cmdContext, VkDeviceSize size) {
    VkCommandBuffer cmd = cmdContext->beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(cmd, srcBuffer->get(), buffer_, 1, &copyRegion);

    cmdContext->endSingleTimeCommands(cmd);
}

void Buffer::map(void** dataOut) {
    vmaMapMemory(device_->getAllocator(), allocation_, dataOut);
}

void Buffer::unmap() {
    vmaUnmapMemory(device_->getAllocator(), allocation_);
}

VkBuffer Buffer::get() const {
    return buffer_;
}

Buffer::~Buffer() {
    if (buffer_ != VK_NULL_HANDLE) {
        vmaDestroyBuffer(device_->getAllocator(), buffer_, allocation_);
    }
}

