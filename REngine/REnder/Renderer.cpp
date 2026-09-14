#include"Renderer.hpp"
#include<cassert>
using namespace render;

void Renderer::init(re::Device* device, re::Swapchain* swapchain, re::RenderPass* renderPass,
    re::Pipeline* pipeline, re::CommandContext* cmdContext, re::FrameContext* frameContext) {
    device_ = device;
    swapchain_ = swapchain;
    renderPass_ = renderPass;
    pipeline_ = pipeline;
    cmdContext_ = cmdContext;
    frameContext_ = frameContext;

    VkDeviceSize bufferSize = sizeof(re::Vertex) * MAX_VERTICES;

    vertexBuffer_.create(device_, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO);

}
void Renderer::beginFrame() {
    frameContext_->beginFrame(swapchain_, &currentImageIndex_);

    currentCmd_ = cmdContext_->getBuffer(frameContext_->getCurrentFrame());
    vkResetCommandBuffer(currentCmd_, 0);

    VkCommandBufferBeginInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkBeginCommandBuffer(currentCmd_, &bufferInfo);

    vertexCount_ = 0;

    VkClearValue clearColor = { {{0.05f,0.05f,0.08f,1.0f}} };
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

    if (vertexCount_ > 0) {
        void* data;
        vertexBuffer_.map(&data);
        memcpy(data, vertexCpuBuffer_, sizeof(re::Vertex) * vertexCount_);
        vertexBuffer_.unmap();

        vkCmdDraw(currentCmd_, vertexCount_, 1, 0, 0);
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

void Renderer::drawTriangle(re::ReVec2 pos, re::ReVec2 size, re::ReColor col) {
    re::Vertex vertices[3] = {
        { { pos.x,          pos.y - size.y }, {col.r,col.g,col.b,col.a} },
        { { pos.x + size.x, pos.y + size.y },  {col.r,col.g,col.b,col.a} },
        { { pos.x - size.x, pos.y + size.y },  {col.r,col.g,col.b,col.a} },
    };
    drawVertices(vertices, 3);
}

void Renderer::drawQuad(re::ReVec2 pos, re::ReVec2 size, re::ReColor col) {
    re::Vertex v1{ { pos.x,          pos.y },           {col.r,col.g,col.b,col.a} };
    re::Vertex v2{ { pos.x + size.x,   pos.y },           {col.r,col.g,col.b,col.a} };
    re::Vertex v3{ { pos.x + size.x,   pos.y + size.y },    {col.r,col.g,col.b,col.a} };
    re::Vertex v4{ { pos.x,          pos.y + size.y },    {col.r,col.g,col.b,col.a} };

    re::Vertex vertices[6] = { v1, v2, v3,  v1, v3, v4 };
    drawVertices(vertices, 6);
}
