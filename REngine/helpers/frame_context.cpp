#include"../REngine.hpp"
using namespace re;



void FrameContext::createSyncObjects(Device* device, size_t frameCount) {

}

void FrameContext::beginFrame(Swapchain* swapchain, uint32_t* imageIndexOut) {

}
void FrameContext::endFrame(Device* device, Swapchain* swapchain, uint32_t imageIndex, VkCommandBuffer cmdBuffer) {

}

void FrameContext::advanceFrame() {

}
size_t FrameContext::getCurrentFrame() const {

}

std::vector<VkFramebuffer> FrameContext::createFramebuffer(Device* device, Swapchain* swapchain, RenderPass* renderPass) {

}

FrameContext::~FrameContext() {

}