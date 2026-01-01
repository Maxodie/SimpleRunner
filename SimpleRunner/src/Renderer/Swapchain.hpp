#pragma once
#include "Renderer/VulkanStructures.hpp"
#include "Renderer/CommandList.hpp"

namespace SR
{

class SwapChain
{
public:
    void Init(RendererContext& context);
    void Shutdown(RendererContext& context);

    void InitSwapChainSupportData(RendererContext& context);
    void ShutdownSwapChainSupportData();

    bool BeginFrame(CommandList& commandList, RendererContext& context);
    void EndFrame(RendererContext& context);
    bool Present(RendererContext& context);

    SR_INLINE nvrhi::FramebufferHandle& GetFrameBufferInFlight()
    {
        return m_data.Framebuffers[m_data.CurrentSwapChainImageId];
    }

    SR_INLINE SwapChainData& GetData()
    {
        return m_data;
    }

    SR_INLINE void Resize(RendererContext& context, uint32_t width, uint32_t height)
    {
        context.Device.waitIdle();
        m_data.Width = width;
        m_data.Height = height;
        DestroyFramebuffers(context);
        Destroy(context);
        Create(context);
        CreateFramebuffers(context);
    }

private:
    void Create(RendererContext& context);
    void Destroy(RendererContext& context);

    void CreateFramebuffers(RendererContext& context);
    void DestroyFramebuffers(RendererContext& context);

    void CreateSemaphores(RendererContext& context);
    void DestroySemaphores(RendererContext& context);

    bool VulkanSwapChainSupportByQuery(RendererContext& context);
    vk::SurfaceFormatKHR VulkanChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
    vk::PresentModeKHR VulkanChooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes);
    vk::Extent2D VulkanChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

    SwapChainData m_data;
};

}
