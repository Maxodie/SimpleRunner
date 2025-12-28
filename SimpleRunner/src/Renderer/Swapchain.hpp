#pragma once
#include "Renderer/VulkanStructures.hpp"

namespace SR
{

class Swapchain
{
public:
    void Init(RendererContext& context);
    void Shutdown(RendererContext& context);

    void InitSwapChainSupportData(RendererContext& context);
    void ShutdownSwapChainSupportData();

    SR_INLINE SwapchainData& GetData() { return m_data; }

    SR_INLINE void Recreate(RendererContext& context)
    {
        Destroy(context);
        Create(context);
    }

private:
    void Create(RendererContext& context);
    void Destroy(RendererContext& context);

    bool VulkanSwapChainSupportByQuery(RendererContext& context);
    void ImagesQuery(RendererContext& context);
    vk::SurfaceFormatKHR VulkanChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
    vk::PresentModeKHR VulkanChooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes);
    vk::Extent2D VulkanChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

    SwapchainData m_data;
};

}
