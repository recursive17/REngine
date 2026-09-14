#include"rengine.hpp"
#include"../REnder/renderer.hpp"

namespace {
    re::Device device;
    re::Swapchain swapchain;
    re::RenderPass renderPass;
    re::Pipeline pipeline;
    re::Pipeline roundPipeline;
    re::CommandContext cmdContext;
    re::FrameContext frameContext;
    render::Renderer renderer;
}

void re::init(GLFWwindow* window) {
    device.createInstance();
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
    roundPipeline.createRound(&device, &renderPass, &swapchain);
    //

    cmdContext.createCommandPool(&device);
    cmdContext.createCommandBuffer(swapchain.getImageCount());

    frameContext.createSyncObjects(&device, swapchain.getImageCount());
    frameContext.createFramebuffer(&device, &swapchain, &renderPass);
    renderer.init(&device, &swapchain, &renderPass, &pipeline, &roundPipeline, &cmdContext, &frameContext);
    
    
}

void re::render() {
    renderer.beginFrame();
    renderer.drawTriangle({ 0.0f, 0.0f }, { 0.3f, 0.3f }, { 135, 255, 13, 255 });
    renderer.drawQuad({ -0.5f, -0.5f }, { 0.3f, 0.2f }, { 12, 25, 255, 255 });
    renderer.drawRoundQuad({ 0.2f, 0.2f }, { 0.4f, 0.4f }, 0.05f, { 12, 25, 255, 255 }); 
    renderer.drawRoundQuad({ 0.5f, -0.4f }, { 0.4f, 0.4f }, 0.1f, { 12, 25, 255, 255 });
    renderer.endFrame();
}
