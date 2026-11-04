#include"renderer.hpp"
#include<cassert>
#include <cstring>  // memcpy
#include <cmath>    // sqrtf
using namespace render;

void Renderer::init(re::Device* device, re::Swapchain* swapchain, re::RenderPass* renderPass,
    re::Pipeline* pipeline, re::Pipeline* roundPipeline, re::Pipeline* imagePipeline, re::Pipeline* triPipeline,
    re::CommandContext* cmdContext, re::FrameContext* frameContext) {
    device_ = device;
    swapchain_ = swapchain;
    renderPass_ = renderPass;
    pipeline_ = pipeline;
    roundPipeline_ = roundPipeline;
    imagePipeline_ = imagePipeline;
    triPipeline_ = triPipeline;
    cmdContext_ = cmdContext;
    frameContext_ = frameContext;

    vertexBuffer_.create(device_, sizeof(re::Vertex) * MAX_VERTICES,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO);
    roundVertexBuffer_.create(device_, sizeof(re::RoundVertex) * MAX_VERTICES,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO);
    imageVertexBuffer_.create(device_, sizeof(re::ImageVertex) * MAX_VERTICES,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO);
    triVertexBuffer_.create(device_, sizeof(re::TriVertex) * MAX_VERTICES,
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
    imageVertexCount_ = 0;
    imageDraws_.clear();
    triVertexCount_ = 0;
   

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

    if (imageVertexCount_ > 0) {
        void* data;
        imageVertexBuffer_.map(&data);
        memcpy(data, imageVertexCpuBuffer_, sizeof(re::ImageVertex) * imageVertexCount_);
        imageVertexBuffer_.unmap();

        vkCmdBindPipeline(currentCmd_, VK_PIPELINE_BIND_POINT_GRAPHICS, imagePipeline_->get());
        VkBuffer buffers[] = { imageVertexBuffer_.get() };
        vkCmdBindVertexBuffers(currentCmd_, 0, 1, buffers, offsets);

        for (const auto& draw : imageDraws_) {
            VkDescriptorSet set = draw.texture->getDescriptorSet();
            vkCmdBindDescriptorSets(currentCmd_, VK_PIPELINE_BIND_POINT_GRAPHICS,
                imagePipeline_->getLayout(), 0, 1, &set, 0, nullptr);

            vkCmdDraw(currentCmd_, 6, 1, draw.firstVertex, 0);
        }
    }

    if (triVertexCount_ > 0) {
        void* data;
        triVertexBuffer_.map(&data);
        memcpy(data, triVertexCpuBuffer_, sizeof(re::TriVertex) * triVertexCount_);
        triVertexBuffer_.unmap();

        vkCmdBindPipeline(currentCmd_, VK_PIPELINE_BIND_POINT_GRAPHICS, triPipeline_->get());
        VkBuffer buffers[] = { triVertexBuffer_.get() };
        vkCmdBindVertexBuffers(currentCmd_, 0, 1, buffers, offsets);
        vkCmdDraw(currentCmd_, triVertexCount_, 1, 0, 0);
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

void Renderer::drawTriVertices(const re::TriVertex* vertices, uint32_t count) {
    assert(triVertexCount_ + count <= MAX_VERTICES && "too many tri vertices");
    memcpy(&triVertexCpuBuffer_[triVertexCount_], vertices, sizeof(re::TriVertex) * count);
    triVertexCount_ += count;
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

void Renderer::drawRoundTriangle(re::ReVec2 pos, re::ReVec2 size, float radius, re::ReCol col) {
    float r = col.getRedf(), g = col.getGreenf(), b = col.getBluef(), a = col.getAlphaf();

    re::ReVec2 t0{ pos.x_,               pos.y_ };
    re::ReVec2 t1{ pos.x_ + size.x_ / 2, pos.y_ + size.y_ };
    re::ReVec2 t2{ pos.x_ - size.x_ / 2, pos.y_ + size.y_ };

    const float PAD = radius + 2.0f;
    re::ReVec2 c{ (t0.x_ + t1.x_ + t2.x_) / 3.0f, (t0.y_ + t1.y_ + t2.y_) / 3.0f };

    auto expand = [&](re::ReVec2 p) {
        float dx = p.x_ - c.x_, dy = p.y_ - c.y_;
        float len = sqrtf(dx * dx + dy * dy);
        if (len < 0.0001f) return pixelToNDC(p);
        return pixelToNDC({ p.x_ + dx / len * PAD, p.y_ + dy / len * PAD });
        };

    re::ReVec2 e0 = expand(t0), e1 = expand(t1), e2 = expand(t2);

    re::TriVertex v0{ { e0.x_, e0.y_ }, {r,g,b,a}, { t0.x_, t0.y_ }, { t1.x_, t1.y_ }, { t2.x_, t2.y_ }, radius };
    re::TriVertex v1{ { e1.x_, e1.y_ }, {r,g,b,a}, { t0.x_, t0.y_ }, { t1.x_, t1.y_ }, { t2.x_, t2.y_ }, radius };
    re::TriVertex v2{ { e2.x_, e2.y_ }, {r,g,b,a}, { t0.x_, t0.y_ }, { t1.x_, t1.y_ }, { t2.x_, t2.y_ }, radius };

    re::TriVertex vertices[3] = { v0, v1, v2 };
    drawTriVertices(vertices, 3);
}

void Renderer::drawQuad(re::ReVec2 pos, re::ReVec2 size, re::ReCol col) {
    float r = col.getRedf(), g = col.getGreenf(), b = col.getBluef(), a = col.getAlphaf();

    re::ReVec2 p1 = pixelToNDC({ pos.x_,           pos.y_ });
    re::ReVec2 p2 = pixelToNDC({ pos.x_ + size.x_, pos.y_ });
    re::ReVec2 p3 = pixelToNDC({ pos.x_ + size.x_, pos.y_ + size.y_ });
    re::ReVec2 p4 = pixelToNDC({ pos.x_,           pos.y_ + size.y_ });

    re::Vertex v1{ { p1.x_, p1.y_ }, { r,g,b,a } };
    re::Vertex v2{ { p2.x_, p2.y_ }, { r,g,b,a } };
    re::Vertex v3{ { p3.x_, p3.y_ }, { r,g,b,a } };
    re::Vertex v4{ { p4.x_, p4.y_ }, { r,g,b,a } };

    re::Vertex vertices[6] = { v1, v2, v3, v1, v3, v4 };
    drawVertices(vertices, 6);
}

void Renderer::drawRoundQuad(re::ReVec2 pos, re::ReVec2 size, float radius, re::ReCol col) {
    float r = col.getRedf(), g = col.getGreenf(), b = col.getBluef(), a = col.getAlphaf();

    const float PAD = 2.0f;

    re::ReVec2 q1 = pixelToNDC({ pos.x_ - PAD,           pos.y_ - PAD });
    re::ReVec2 q2 = pixelToNDC({ pos.x_ + size.x_ + PAD, pos.y_ - PAD });
    re::ReVec2 q3 = pixelToNDC({ pos.x_ + size.x_ + PAD, pos.y_ + size.y_ + PAD });
    re::ReVec2 q4 = pixelToNDC({ pos.x_ - PAD,           pos.y_ + size.y_ + PAD });

    float u0 = -PAD / size.x_, u1 = 1.0f + PAD / size.x_;
    float v0 = -PAD / size.y_, v1 = 1.0f + PAD / size.y_;

    re::RoundVertex vA{ { q1.x_, q1.y_ }, {r,g,b,a}, { u0, v0 }, { size.x_, size.y_ }, radius };
    re::RoundVertex vB{ { q2.x_, q2.y_ }, {r,g,b,a}, { u1, v0 }, { size.x_, size.y_ }, radius };
    re::RoundVertex vC{ { q3.x_, q3.y_ }, {r,g,b,a}, { u1, v1 }, { size.x_, size.y_ }, radius };
    re::RoundVertex vD{ { q4.x_, q4.y_ }, {r,g,b,a}, { u0, v1 }, { size.x_, size.y_ }, radius };

    re::RoundVertex vertices[6] = { vA, vB, vC, vA, vC, vD };
    drawRoundVertices(vertices, 6);
}

void Renderer::drawImage(re::Texture* texture, re::ReVec2 pos, re::ReVec2 size,
    float radius, re::ReCol tint) {
    if (!texture) return;
    assert(imageVertexCount_ + 6 <= MAX_VERTICES && "too many image vertices");

    float r = tint.getRedf(), g = tint.getGreenf(), b = tint.getBluef(), a = tint.getAlphaf();

    re::ReVec2 p1 = pixelToNDC({ pos.x_,           pos.y_ });
    re::ReVec2 p2 = pixelToNDC({ pos.x_ + size.x_, pos.y_ });
    re::ReVec2 p3 = pixelToNDC({ pos.x_ + size.x_, pos.y_ + size.y_ });
    re::ReVec2 p4 = pixelToNDC({ pos.x_,           pos.y_ + size.y_ });

    re::ImageVertex v1{ { p1.x_, p1.y_ }, { 0.0f, 0.0f }, { size.x_, size.y_ }, radius, { r,g,b,a } };
    re::ImageVertex v2{ { p2.x_, p2.y_ }, { 1.0f, 0.0f }, { size.x_, size.y_ }, radius, { r,g,b,a } };
    re::ImageVertex v3{ { p3.x_, p3.y_ }, { 1.0f, 1.0f }, { size.x_, size.y_ }, radius, { r,g,b,a } };
    re::ImageVertex v4{ { p4.x_, p4.y_ }, { 0.0f, 1.0f }, { size.x_, size.y_ }, radius, { r,g,b,a } };

    imageDraws_.push_back({ texture, imageVertexCount_ });

    re::ImageVertex quad[6] = { v1, v2, v3, v1, v3, v4 };
    memcpy(&imageVertexCpuBuffer_[imageVertexCount_], quad, sizeof(quad));
    imageVertexCount_ += 6;
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

