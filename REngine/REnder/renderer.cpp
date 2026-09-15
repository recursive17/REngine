#include"renderer.hpp"
#include<cassert>
using namespace render;

void Renderer::init(re::Device* device, re::Swapchain* swapchain, re::RenderPass* renderPass,
    re::Pipeline* pipeline, re::Pipeline* roundPipeline,
    re::CommandContext* cmdContext, re::FrameContext* frameContext) {
    device_ = device;
    swapchain_ = swapchain;
    renderPass_ = renderPass;
    pipeline_ = pipeline;
    roundPipeline_ = roundPipeline;
    cmdContext_ = cmdContext;
    frameContext_ = frameContext;

    vertexBuffer_.create(device_, sizeof(re::Vertex) * MAX_VERTICES,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO);
    roundVertexBuffer_.create(device_, sizeof(re::RoundVertex) * MAX_VERTICES,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO);
}

void Renderer::beginFrame() {
    frameContext_->beginFrame(swapchain_, &currentImageIndex_);

    currentCmd_ = cmdContext_->getBuffer(frameContext_->getCurrentFrame());
    vkResetCommandBuffer(currentCmd_, 0);

    VkCommandBufferBeginInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkBeginCommandBuffer(currentCmd_, &bufferInfo);

    vertexCount_ = 0;
    roundVertexCount_ = 0;                                  

    VkClearValue clearColor = { {{0.05f, 0.05f, 0.08f, 1.0f}} };
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass_->get();
    renderPassInfo.framebuffer = frameContext_->getFramebuffer(currentImageIndex_);
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapchain_->getExtent();
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(currentCmd_, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkExtent2D extent = swapchain_->getExtent();
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(currentCmd_, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = extent;
    vkCmdSetScissor(currentCmd_, 0, 1, &scissor);

    vkCmdBindPipeline(currentCmd_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_->get());
    VkBuffer buffers[] = { vertexBuffer_.get() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(currentCmd_, 0, 1, buffers, offsets);

}

void Renderer::endFrame() {
    VkDeviceSize offsets[] = { 0 };

    if (vertexCount_ > 0) {
        void* data;
        vertexBuffer_.map(&data);
        memcpy(data, vertexCpuBuffer_, sizeof(re::Vertex) * vertexCount_);
        vertexBuffer_.unmap();

        vkCmdBindPipeline(currentCmd_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_->get());
        VkBuffer buffers[] = { vertexBuffer_.get() };
        vkCmdBindVertexBuffers(currentCmd_, 0, 1, buffers, offsets);

        vkCmdDraw(currentCmd_, vertexCount_, 1, 0, 0);
    }

    if (roundVertexCount_ > 0) {
        void* data;
        roundVertexBuffer_.map(&data);
        memcpy(data, roundVertexCpuBuffer_, sizeof(re::RoundVertex) * roundVertexCount_);
        roundVertexBuffer_.unmap();

        vkCmdBindPipeline(currentCmd_, VK_PIPELINE_BIND_POINT_GRAPHICS, roundPipeline_->get());
        VkBuffer buffers[] = { roundVertexBuffer_.get() };
        vkCmdBindVertexBuffers(currentCmd_, 0, 1, buffers, offsets);

        vkCmdDraw(currentCmd_, roundVertexCount_, 1, 0, 0);
    }

    vkCmdEndRenderPass(currentCmd_);
    vkEndCommandBuffer(currentCmd_);

    frameContext_->endFrame(device_, swapchain_, currentImageIndex_, currentCmd_);
    frameContext_->advanceFrame();
}

void Renderer::drawVertices(const re::Vertex* vertices, uint32_t count) {
    assert(count <= MAX_VERTICES && "too many vertices for buffer");
    memcpy(&vertexCpuBuffer_[vertexCount_], vertices, sizeof(re::Vertex) * count);
    vertexCount_ += count;
}

void Renderer::drawRoundVertices(const re::RoundVertex* vertices, uint32_t count) {
    assert(roundVertexCount_ + count <= MAX_VERTICES && "too many round vertices");
    memcpy(&roundVertexCpuBuffer_[roundVertexCount_], vertices, sizeof(re::RoundVertex) * count);
    roundVertexCount_ += count;
}

void Renderer::drawTriangle(re::ReVec2 pos, re::ReVec2 size, re::ReCol col) {
    float r = col.getRedf(), g = col.getGreenf(), b = col.getBluef(), a = col.getAlphaf();


    //HERE IS METHOD, YOU NEED TO READ THIS METHOD
    re::ReVec2 p1 = pixelToNDC({ pos.x_,               pos.y_ });
    re::ReVec2 p2 = pixelToNDC({ pos.x_ + size.x_ / 2, pos.y_ + size.y_ });
    re::ReVec2 p3 = pixelToNDC({ pos.x_ - size.x_ / 2, pos.y_ + size.y_ });

    re::Vertex vertices[3] = {
        { { p1.x_, p1.y_ }, { r, g, b, a } },
        { { p3.x_, p3.y_ }, { r, g, b, a } }, 
        { { p2.x_, p2.y_ }, { r, g, b, a } },
    };
    drawVertices(vertices, 3);
}

void Renderer::drawQuad(re::ReVec2 pos, re::ReVec2 size, re::ReCol col) {
    float r = col.getRedf(), g = col.getGreenf(), b = col.getBluef(), a = col.getAlphaf();

    re::Vertex v1{ { pos.x_,           pos.y_ },           {r,g,b,a} };
    re::Vertex v2{ { pos.x_ + size.x_, pos.y_ },           {r,g,b,a} };
    re::Vertex v3{ { pos.x_ + size.x_, pos.y_ + size.y_ }, {r,g,b,a} };
    re::Vertex v4{ { pos.x_,           pos.y_ + size.y_ }, {r,g,b,a} };

    re::Vertex vertices[6] = { v1, v2, v3, v1, v3, v4 };
    drawVertices(vertices, 6);
}

void Renderer::drawRoundQuad(re::ReVec2 pos, re::ReVec2 size, float radius, re::ReCol col) {
    float r = col.getRedf(), g = col.getGreenf(), b = col.getBluef(), a = col.getAlphaf();

    re::RoundVertex v1{ { pos.x_,           pos.y_           }, { r, g, b, a }, { 0.0f, 0.0f }, { size.x_, size.y_ }, radius };
    re::RoundVertex v2{ { pos.x_ + size.x_, pos.y_           }, { r, g, b, a }, { 1.0f, 0.0f }, { size.x_, size.y_ }, radius };
    re::RoundVertex v3{ { pos.x_ + size.x_, pos.y_ + size.y_ }, { r, g, b, a }, { 1.0f, 1.0f }, { size.x_, size.y_ }, radius };
    re::RoundVertex v4{ { pos.x_,           pos.y_ + size.y_ }, { r, g, b, a }, { 0.0f, 1.0f }, { size.x_, size.y_ }, radius };

    re::RoundVertex vertices[6] = { v1, v2, v3, v1, v3, v4 };
    drawRoundVertices(vertices, 6);
}


// --- UTILS ---
//HERE
re::ReVec2 Renderer::pixelToNDC(const re::ReVec2 pixelPos) const {
    VkExtent2D extent = swapchain_->getExtent();
    float w = static_cast<float>(extent.width);
    float h = static_cast<float>(extent.height);
    return {
        (2.0f * pixelPos.x_ / w) - 1.0f,
        (2.0f * pixelPos.y_ / h) - 1.0f 
    };
}

