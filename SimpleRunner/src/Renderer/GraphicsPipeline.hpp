#pragma once
#include "Renderer/VulkanStructures.hpp"

namespace SR
{

class GraphicsPipeline
{
public:
    void Create(RendererContext& context);
    void Destroy(RendererContext& context);

    SR_INLINE GraphicsPipelineData& GetData()
    {
        return m_data;
    }

private:
    GraphicsPipelineData m_data;
};

}
