#include"REngine.hpp"


namespace {
    re::Device device;
    re::Swapchain swapchain;
    re::RenderPass renderPass;
    re::Pipeline pipeline;
    re::CommandContext cmdContext;
    re::FrameContext frameContext;
}

void re::init(GLFWwindow* window) {
    device.createInstance(window);
    device.createSurface(window);
    device.pickPhysicalDevice();
    device.findQueueFamilies();
    device.createLogicalDevice();

    swapchain.create(&device, window);
    swapchain.createImageViews();

    renderPass.create(&device, &swapchain);

    pipeline.create(&device, &renderPass, &swapchain);

    cmdContext.createCommandPool(&device);
    cmdContext.createCommandBuffer(swapchain.getImageCount());

    frameContext.createSyncObjects(&device, swapchain.getImageCount());
    frameContext.createFramebuffer(&device, &swapchain, &renderPass);
}

void re::render() {

}