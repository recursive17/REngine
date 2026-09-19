#include"rengine.hpp"
#include"../REnder/renderer.hpp"
#include"assets/logo.h"
#include"audio.hpp"
#include"assets/click.h"

namespace {
    re::Device device;
    re::Swapchain swapchain;
    re::RenderPass renderPass;
    re::Pipeline pipeline;
    re::Pipeline roundPipeline;
    re::Pipeline triRoundPipeline;
    re::CommandContext cmdContext;
    re::TextureManager textureManager;
    re::Pipeline imagePipeline;
    re::FrameContext frameContext;
    render::Renderer renderer;
    re::SoundManager soundManager;
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

    triRoundPipeline.createRoundTri(&device, &renderPass, &swapchain);
        re::ReLog::success(re::Prefix::VULKAN, "tri round pipeline is created");

    cmdContext.createCommandPool(&device);
    cmdContext.createCommandBuffer(swapchain.getImageCount());
        re::ReLog::success(re::Prefix::VULKAN, "class commandContext is created!");

    textureManager.init(&device, &cmdContext);
    imagePipeline.createImage(&device, &renderPass, &swapchain, textureManager.getLayout());
        re::ReLog::success(re::Prefix::VULKAN, "image pipeline is created");

    textureManager.load("test", "textures/other/test.jpg");
    textureManager.loadFromMemory("logo", re::assets::logoPng());


    frameContext.createSyncObjects(&device, swapchain.getImageCount());
    frameContext.createFramebuffer(&device, &swapchain, &renderPass);
        re::ReLog::success(re::Prefix::VULKAN, "class framebuffer is created!");

    soundManager.init();
    soundManager.loadFromMemory("click", re::assets::clickWav());
    soundManager.play("click");

    renderer.init(&device, &swapchain, &renderPass, &pipeline, &roundPipeline, &imagePipeline, &triRoundPipeline, &cmdContext, &frameContext);
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

    renderer.drawImage(textureManager.get("test"), { 50, 420 }, { 128, 128 });
    renderer.drawImage(textureManager.get("logo"), { 200, 420 }, { 128, 128 });
    renderer.drawImage(textureManager.get("logo"), { 350, 420 }, { 128, 128 }, 24);

    renderer.endFrame();
}
