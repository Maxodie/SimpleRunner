#pragma once
#include "Core/Core.hpp"
#include "Renderer/VulkanStructures.hpp"
#include "nvrhi/nvrhi.h"

namespace SR
{

class CommandList
{
public:
    SR_INLINE void Create(RendererContext& context)
    {
        m_handle = context.GetHandle()->createCommandList();
    }

    SR_INLINE void Destroy()
    {
        m_handle = nullptr;
    }

    SR_INLINE nvrhi::CommandListHandle& GetHandle()
    {
        CORE_ASSERT(m_handle, "trying to get command list handle before creating it");
        return m_handle;
    }

private:
    nvrhi::CommandListHandle m_handle = nullptr;
};

}
