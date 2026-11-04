#pragma once
#include "../REngine/rengine.hpp"
#include "../REngine/utils/revec.hpp"
#include "../REngine/utils/recol.hpp"

namespace render {
    class Renderer {
    public:
        void init(re::Device* device, re::Swapchain* swapchain, re::RenderPass* renderPass,
            re::Pipeline* pipeline, re::Pipeline* roundPipeline, re::Pipeline* imagePipeline, re::Pipeline* triPipeline, re::CommandContext* cmdContext, re::FrameContext* frameContext);

        void beginFrame();
        void endFrame();

        void drawTriangle(re::ReVec2 pos, re::ReVec2 size, re::ReCol col);
        void drawQuad(re::ReVec2 pos, re::ReVec2 size, re::ReCol col);
        void drawRoundQuad(re::ReVec2 pos, re::ReVec2 size, float radius, re::ReCol col);
        void drawImage(re::Texture* texture, re::ReVec2 pos, re::ReVec2 size, float radius = 0.0f, re::ReCol tint = { 255, 255, 255, 255 });
        void drawRoundTriangle(re::ReVec2 pos, re::ReVec2 size, float radius, re::ReCol col);

    private:
        struct ImageDraw {
            re::Texture* texture;
            uint32_t firstVertex;
        };

        void drawVertices(const re::Vertex* vertices, uint32_t count);
        void drawRoundVertices(const re::RoundVertex* vertices, uint32_t count);
        void drawTriVertices(const re::TriVertex* vertices, uint32_t count);

        re::ReVec2 pixelToNDC(const re::ReVec2 pixelPos) const;

        re::Device*                 device_             { nullptr };
        re::Swapchain*              swapchain_          { nullptr };
        re::RenderPass*             renderPass_         { nullptr };
        re::Pipeline*               pipeline_           { nullptr };
        re::Pipeline*               roundPipeline_      { nullptr };
        re::Pipeline*               imagePipeline_      { nullptr };
        re::Pipeline*               triPipeline_        { nullptr };
        re::CommandContext*         cmdContext_         { nullptr };
        re::FrameContext*           frameContext_       { nullptr };

        re::Buffer                  vertexBuffer_;
        re::Buffer                  roundVertexBuffer_;
        re::Buffer                  imageVertexBuffer_;
        re::Buffer                  triVertexBuffer_;

        static constexpr uint32_t   MAX_VERTICES         = 3000;

        re::Vertex                  vertexCpuBuffer_    [MAX_VERTICES];
        uint32_t                    vertexCount_        { 0 };
        re::RoundVertex             roundVertexCpuBuffer_[MAX_VERTICES];
        uint32_t                    roundVertexCount_   { 0 };
        re::ImageVertex             imageVertexCpuBuffer_[MAX_VERTICES];
        uint32_t                    imageVertexCount_   { 0 };
        re::TriVertex               triVertexCpuBuffer_[MAX_VERTICES];
        uint32_t                    triVertexCount_     { 0 };

        std::vector<ImageDraw>      imageDraws_;
        uint32_t                    currentImageIndex_  { 0 };
        VkCommandBuffer             currentCmd_         { VK_NULL_HANDLE };
    };
}
