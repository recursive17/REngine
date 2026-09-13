#include"../REngine.hpp"
using namespace re;



void CommandContext::createCommandPool(Device* device) {
	device_ = device;

	VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = device_->getGraphicsQueueFamily();
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(device_->get(), &poolInfo, nullptr, &commandPool_) != VK_SUCCESS) {
//TODO...
	}
}
void CommandContext::createCommandBuffer(size_t count) {
	commandBuffer_.resize(count);

	VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool_;
		allocInfo.commandBufferCount = static_cast<uint32_t>(count);
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	if (vkAllocateCommandBuffers(device_->get(), &allocInfo, commandBuffer_.data())) {
//TODO ADD LOGG..
	}
}

VkCommandBuffer CommandContext::beginSingleTimeCommands() {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool_;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device_->get(), &allocInfo, &commandBuffer);
}
void CommandContext::endSingleTimeCommands(VkCommandBuffer cmdBuffer) {

}

VkCommandPool CommandContext::getPool() const {

}
VkCommandBuffer CommandContext::getBuffer(size_t index) const {

}

CommandContext::~CommandContext() {

}