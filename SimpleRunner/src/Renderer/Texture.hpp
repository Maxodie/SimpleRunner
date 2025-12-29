#pragma once
#include "Renderer/VulkanInclude.hpp"

namespace SR
{

struct RendererContext;

class Texture
{
public:
    struct Data
    {
        uint32_t Width, Height;
    };

    void Create(const Data& data);
    void Create(RendererContext& context, const Data& data);

    SR_INLINE nvrhi::TextureHandle& GetHandle()
    {
        CORE_ASSERT(m_textureHandle, "Trying to get the texture handle before creating it");
        return m_textureHandle;
    }

private:
    nvrhi::TextureHandle m_textureHandle = nullptr;
    Data m_data;
};

}
