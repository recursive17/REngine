#include"REngine.hpp"
#include"../REnder/Renderer.hpp"

namespace {
    re::Device device;
    re::Swapchain swapchain;
    re::RenderPass renderPass;
    re::Pipeline pipeline;
    re::CommandContext cmdContext;
    re::FrameContext frameContext;
    render::Renderer renderer;
}

void re::init(GLFWwindow* window) {
    device.createInstance(window);
    device.createSurface(window);
    device.pickPhysicalDevice();
    device.findQueueFamilies();
    device.createLogicalDevice();
    device.createAllocator();

    swapchain.create(&device, window);
    swapchain.createImageViews();

    renderPass.create(&device, &swapchain);

    //shaders
        pipeline.createSolid(&device, &renderPass, &swapchain);

    //

    cmdContext.createCommandPool(&device);
    cmdContext.createCommandBuffer(swapchain.getImageCount());

    frameContext.createSyncObjects(&device, swapchain.getImageCount());
    frameContext.createFramebuffer(&device, &swapchain, &renderPass);
    renderer.init(&device, &swapchain, &renderPass, &pipeline, &cmdContext, &frameContext);
    
    
}

void re::render() {
    renderer.beginFrame();
    renderer.drawTriangle({ 0.0f, 0.0f }, { 0.3f, 0.3f }, { 1.0f, 0.2f, 0.2f, 1.0f });
    renderer.drawQuad({ -0.5f, -0.5f }, { 0.3f, 0.2f }, { 0.2f, 1.0f, 0.2f, 1.0f });
    renderer.endFrame();
}