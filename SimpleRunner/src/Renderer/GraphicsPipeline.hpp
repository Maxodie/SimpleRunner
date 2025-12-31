#pragma once
#include "Renderer/VulkanStructures.hpp"

namespace SR
{

class GraphicsPipeline
{
public:
    bool Create(RendererContext& context, const nvrhi::FramebufferInfo& info);
    void Destroy(RendererContext& context);

    SR_INLINE GraphicsPipelineData& GetData()
    {
        return m_data;
    }

private:
    GraphicsPipelineData m_data;
};

}
