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
        re::ReLog::success(re::Prefix::VULKAN, "class device is created!");

    swapchain.create(&device, window);
    swapchain.createImageViews();
        re::ReLog::success(re::Prefix::VULKAN, "class swapchain is created!");

    renderPass.create(&device, &swapchain);
        re::ReLog::success(re::Prefix::VULKAN, "class renderPass is created!");

    //shaders

    pipeline.createSolid(&device, &renderPass, &swapchain);
        re::ReLog::success(re::Prefix::VULKAN, "solid pipeline is created");

    roundPipeline.createRound(&device, &renderPass, &swapchain);
        re::ReLog::success(re::Prefix::VULKAN, "round pipeline is created");

    //

    cmdContext.createCommandPool(&device);
    cmdContext.createCommandBuffer(swapchain.getImageCount());
        re::ReLog::success(re::Prefix::VULKAN, "class commandContext is created!");


    frameContext.createSyncObjects(&device, swapchain.getImageCount());
    frameContext.createFramebuffer(&device, &swapchain, &renderPass);
        re::ReLog::success(re::Prefix::VULKAN, "class framebuffer is created!");

    renderer.init(&device, &swapchain, &renderPass, &pipeline, &roundPipeline, &cmdContext, &frameContext);
    
    
}

void re::render() {
    renderer.beginFrame();

    //I MADE IT
    renderer.drawTriangle({ 200.0f, 200.0f }, { 100.0f, 100.0f}, { 135, 255, 13, 255 });
    //
    renderer.drawQuad({ -0.5f, -0.5f }, { 0.3f, 0.2f }, { 12, 25, 255, 255 });
    renderer.drawRoundQuad({ 0.2f, 0.2f }, { 0.4f, 0.4f }, 0.05f, { 12, 25, 255, 90 }); 
    renderer.drawRoundQuad({ 0.5f, -0.4f }, { 0.4f, 0.4f }, 0.1f, { 12, 25, 255, 255 });
    renderer.endFrame();
}
