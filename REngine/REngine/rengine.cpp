#include"rengine.hpp"
#include"../REnder/renderer.hpp"

namespace {
    re::Device device;
    re::Swapchain swapchain;
    re::RenderPass renderPass;
    re::Pipeline pipeline;
    re::Pipeline roundPipeline;
    re::Pipeline triRoundPipeline;
    re::CommandContext cmdContext;
    re::FrameContext frameContext;
    render::Renderer renderer;
}

void re::init(GLFWwindow* window) {

    device.createInstance();
    device.createDebugMessenger();
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
    

    cmdContext.createCommandPool(&device);
    cmdContext.createCommandBuffer(swapchain.getImageCount());
        re::ReLog::success(re::Prefix::VULKAN, "class commandContext is created!");


    frameContext.createSyncObjects(&device, swapchain.getImageCount());
    frameContext.createFramebuffer(&device, &swapchain, &renderPass);
        re::ReLog::success(re::Prefix::VULKAN, "class framebuffer is created!");

    renderer.init(&device, &swapchain, &renderPass, &pipeline, &roundPipeline, &triRoundPipeline, &cmdContext, &frameContext);
}

void re::render() {
    renderer.beginFrame();

    //
    renderer.drawQuad({ 50, 50 }, { 100, 60 }, { 12, 25, 255, 255 });
    renderer.drawRoundQuad({ 200, 50 }, { 100, 100 }, 12, { 192, 21, 11, 255 });
    renderer.drawRoundQuad({ 350, 50 }, { 100, 100 }, 50, { 20, 200, 90, 255 });  
    renderer.drawRoundTriangle({ 200, 250 }, { 120, 120 }, 0, { 12, 25, 255, 255 });
    renderer.drawRoundTriangle({ 350, 250 }, { 120, 120 }, 8, { 12, 25, 255, 255 });
    renderer.drawRoundTriangle({ 500, 250 }, { 120, 120 }, 25, { 12, 25, 255, 255 });

    renderer.endFrame();
}
