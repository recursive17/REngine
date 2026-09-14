#pragma once
#include "../REngine/REngine.hpp"
#include "../REngine/utils/revec.hpp"
#include "../REngine/utils/recol.hpp"

namespace render {
    class Renderer {
    public:
        void init(re::Device* device, re::Swapchain* swapchain, re::RenderPass* renderPass,
            re::Pipeline* pipeline, re::CommandContext* cmdContext, re::FrameContext* frameContext);

        void beginFrame();
        void endFrame();

        void drawTriangle(re::ReVec2 pos, re::ReVec2 size, re::ReCol col);
        void drawQuad(re::ReVec2 pos, re::ReVec2 size, re::ReCol col);

    private:
        void drawVertices(const re::Vertex* vertices, uint32_t count);

        re::Device*                 device_             { nullptr };
        re::Swapchain*              swapchain_          { nullptr };
        re::RenderPass*             renderPass_         { nullptr };
        re::Pipeline*               pipeline_           { nullptr };
        re::CommandContext*         cmdContext_         { nullptr };
        re::FrameContext*           frameContext_       { nullptr };

        re::Buffer                  vertexBuffer_;
        static constexpr uint32_t   MAX_VERTICES         = 3000;

        re::Vertex                  vertexCpuBuffer_    [MAX_VERTICES];
        uint32_t                    vertexCount_        { 0 };

        uint32_t                    currentImageIndex_  { 0 };
        VkCommandBuffer             currentCmd_         { VK_NULL_HANDLE };
    };
}
