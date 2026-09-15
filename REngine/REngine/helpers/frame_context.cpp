#include"../rengine.hpp"
using namespace re;



void FrameContext::createSyncObjects(Device* device, size_t frameCount) {
	device_ = device;
    imageAvaiableSemaphores_.resize(frameCount);
    renderFinishedSemaphores_.resize(frameCount);
    inFlightFence_.resize(frameCount);

    VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < frameCount; ++i) {
        if (
            vkCreateSemaphore(device->get(), &semaphoreInfo, nullptr, &imageAvaiableSemaphores_[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device->get(), &semaphoreInfo, nullptr, &renderFinishedSemaphores_[i]) != VK_SUCCESS ||
            vkCreateFence(device->get(), &fenceInfo, nullptr, &inFlightFence_[i]) != VK_SUCCESS
            ) {
            re::ReLog::error(re::Prefix::VULKAN, std::runtime_error("failed to create sync objects for a frame"));

        }
    }

}

void FrameContext::beginFrame(Swapchain* swapchain, uint32_t* imageIndexOut) {
    vkWaitForFences(device_->get(), 1, &inFlightFence_[currentFrame_], VK_TRUE, UINT64_MAX);

    VkResult result = vkAcquireNextImageKHR(
        device_->get(), swapchain->get(), UINT64_MAX,
        imageAvaiableSemaphores_[currentFrame_], VK_NULL_HANDLE, imageIndexOut
    );

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
// TODO: SIGNAL UP NEED RECREATE SWAPCHAIN
        re::ReLog::warning(re::Prefix::VULKAN, "need recreate swapchain");
        return;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        re::ReLog::error(re::Prefix::VULKAN, std::runtime_error("failed to acquire swapchain image"));
    }

    vkResetFences(device_->get(), 1, &inFlightFence_[currentFrame_]);
}
void FrameContext::endFrame(Device* device, Swapchain* swapchain, uint32_t imageIndex, VkCommandBuffer cmdBuffer) {
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &imageAvaiableSemaphores_[currentFrame_];
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderFinishedSemaphores_[currentFrame_];

    if (vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo, inFlightFence_[currentFrame_]) != VK_SUCCESS) {
        re::ReLog::error(re::Prefix::VULKAN, std::runtime_error("failed to submit draw command buffer"));
    }

    VkSwapchainKHR swapchainHandle = swapchain->get();

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderFinishedSemaphores_[currentFrame_];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchainHandle;
    presentInfo.pImageIndices = &imageIndex;

    VkResult result = vkQueuePresentKHR(device->getPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
// TODO: SIGNAL RECREATE
    }
}

void FrameContext::advanceFrame() {
    currentFrame_ = (currentFrame_ + 1) % imageAvaiableSemaphores_.size();
}

size_t FrameContext::getCurrentFrame() const {
    return currentFrame_;
}

std::vector<VkFramebuffer> FrameContext::createFramebuffer(Device* device, Swapchain* swapchain, RenderPass* renderPass) {
    const auto& imageViews = swapchain->getImageViews();
    VkExtent2D extent = swapchain->getExtent();

    framebuffers_.resize(imageViews.size());
    for (size_t i = 0; i < imageViews.size(); ++i){
        VkImageView attachments[] = { imageViews[i] };

        VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass->get();
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = extent.width;
            framebufferInfo.height = extent.height;
            framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device_->get(), &framebufferInfo, nullptr, &framebuffers_[i]) != VK_SUCCESS) {
            re::ReLog::error(re::Prefix::VULKAN, std::runtime_error("failed to create framebuffer"));
        }
    }
    return framebuffers_;
}

FrameContext::~FrameContext() {
    for (auto framebuffer : framebuffers_) {
        vkDestroyFramebuffer(device_->get(), framebuffer, nullptr);
    }
    for (auto semaphore : renderFinishedSemaphores_) {
        vkDestroySemaphore(device_->get(), semaphore, nullptr);
    }
    for (auto semaphore : imageAvaiableSemaphores_) {
        vkDestroySemaphore(device_->get(), semaphore, nullptr);
    }
    for (auto fence : inFlightFence_) {
        vkDestroyFence(device_->get(), fence, nullptr);
    }
}

VkFramebuffer FrameContext::getFramebuffer(uint32_t index) const {
    return framebuffers_[index];
}