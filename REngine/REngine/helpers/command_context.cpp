#include"../rengine.hpp"
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

	VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}
void CommandContext::endSingleTimeCommands(VkCommandBuffer cmdBuffer) {
	vkEndCommandBuffer(cmdBuffer);

	VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdBuffer;


	VkQueue graphicsQueue = device_->getGraphicsQueue();
	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(device_->get(), commandPool_, 1, &cmdBuffer);
}

VkCommandPool CommandContext::getPool() const {
	return commandPool_;
}
VkCommandBuffer CommandContext::getBuffer(size_t index) const {
	return commandBuffer_[index];
}

CommandContext::~CommandContext() {
	if (commandPool_ != VK_NULL_HANDLE) {
		vkDestroyCommandPool(device_->get(), commandPool_, nullptr);
	}
}