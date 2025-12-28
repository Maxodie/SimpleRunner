#include "Renderer/Swapchain.hpp"
#include "nvrhi/common/resource.h"
#include <vulkan/vulkan_core.h>

namespace SR
{

void Swapchain::Init(RendererContext& context)
{
    InitSwapChainSupportData(context);

    VulkanSwapChainSupportByQuery(context);

    m_data.ImageCount = m_data.Capabilitites.minImageCount + 1;

    if (m_data.Capabilitites.maxImageCount > 0 && m_data.ImageCount > m_data.Capabilitites.maxImageCount)// if maxImageCount == 0 then imageCount is illimited
    {
        m_data.ImageCount = m_data.Capabilitites.maxImageCount;
    }

    Create(context);
    ImagesQuery(context);
}

void Swapchain::Shutdown(RendererContext& context)
{
    Destroy(context);
    ShutdownSwapChainSupportData();
}

void Swapchain::InitSwapChainSupportData(RendererContext& context)
{
    uint32_t formatCount;
    vk::Result result = context.PhysicalDevice.getSurfaceFormatsKHR(context.Surface, &formatCount, nullptr);
    CORE_ASSERT_PARAMS(result == vk::Result::eSuccess, "failed to get surface formats code: %d", result);

    if (formatCount)
    {
        m_data.SurfaceFormats.resize(formatCount);
    }

    uint32_t presentModeCount;
    result = context.PhysicalDevice.getSurfacePresentModesKHR(context.Surface, &presentModeCount, nullptr);
    CORE_ASSERT_PARAMS(result == vk::Result::eSuccess, "failed to get surface present modes code: %d", result);

    if (presentModeCount)
    {
        m_data.SurfacePresentModes.resize(presentModeCount);
    }
}

void Swapchain::ShutdownSwapChainSupportData()
{
    m_data.SurfacePresentModes.clear();
    m_data.SurfaceFormats.clear();
}

void Swapchain::Create(RendererContext& context)
{

    vk::SwapchainCreateInfoKHR createInfo{};
    createInfo.surface = context.Surface;
    createInfo.minImageCount = m_data.ImageCount;
    createInfo.imageFormat = m_data.ImageFormat;
    createInfo.imageColorSpace = m_data.ImageColorSpace;
    createInfo.imageExtent = m_data.ImageExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = m_data.ImageUsage;
    createInfo.preTransform = m_data.Capabilitites.currentTransform;
    createInfo.compositeAlpha = m_data.AlphaFlagBitsKHR;
    createInfo.presentMode = m_data.PresentMode;
    createInfo.clipped = true;
    createInfo.oldSwapchain = m_data.Swapchain;

    const uint32_t queueFamilyIndices[] = { context.GraphicsFamily.value(), context.PresentFamily.value() };

    if (context.GraphicsFamily.value() != context.PresentFamily.value())
    {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        createInfo.queueFamilyIndexCount = 0; // Optionnal
        createInfo.pQueueFamilyIndices = nullptr; // Optionnal
    }

    m_data.Swapchain = context.Device.createSwapchainKHR(createInfo);

    auto textureDesc = nvrhi::TextureDesc()
        .setDimension(nvrhi::TextureDimension::Texture2D)
        .setFormat(nvrhi::Format::RGBA8_UNORM)
        .setWidth(m_data.Width)
        .setHeight(m_data.Height)
        .setIsRenderTarget(true)
        .setDebugName("Swap Chain Image");

    // In this line, <type> depends on the GAPI and should be one of: D3D11_Resource, D3D12_Resource, VK_Image.

    m_data.Textures.resize(m_data.Images.size());
    for(size_t i = 0; i < m_data.Images.size(); ++i)
    {
        m_data.Textures[i] = context.DeviceHandle->createHandleForNativeTexture(nvrhi::ObjectTypes::VK_Image, nvrhi::Object(m_data.Images[i]), textureDesc);
    }
}

void Swapchain::Destroy(RendererContext& context)
{
    context.Device.destroySwapchainKHR(m_data.Swapchain);

    m_data.Images.clear();
    m_data.Textures.clear();
}

bool Swapchain::VulkanSwapChainSupportByQuery(RendererContext& context)
{
    //Set details : capabilities, format and presentMode
    vk::Result result = context.PhysicalDevice.getSurfaceCapabilitiesKHR(context.Surface, &m_data.Capabilitites);
    CORE_ASSERT_PARAMS(result == vk::Result::eSuccess, "failed to get surface capabilities code: %d", result);

    //Formats
    if (m_data.SurfaceFormats.size())
    {
        uint32_t surfaceFormatCount = m_data.SurfaceFormats.size();
        vk::Result result = context.PhysicalDevice.getSurfaceFormatsKHR(context.Surface, &surfaceFormatCount, m_data.SurfaceFormats.data());
        CORE_ASSERT_PARAMS(result == vk::Result::eSuccess, "failed to get surface formats code: %d", result);
        CORE_ASSERT_PARAMS(surfaceFormatCount == m_data.SurfaceFormats.size(), "surface format count changed from %zu to %zu", m_data.SurfaceFormats.size(), surfaceFormatCount);
    }

    //Present modes
    if (m_data.SurfacePresentModes.size())
    {
        uint32_t surfacePresentModeCount = m_data.SurfacePresentModes.size();
        vk::Result result = context.PhysicalDevice.getSurfacePresentModesKHR(context.Surface, &surfacePresentModeCount, m_data.SurfacePresentModes.data());
        CORE_ASSERT_PARAMS(result == vk::Result::eSuccess, "failed to get surface formats code: %d", result);
        CORE_ASSERT_PARAMS(surfacePresentModeCount == m_data.SurfacePresentModes.size(), "surface format count changed from %zu to %zu", m_data.SurfacePresentModes.size(), surfacePresentModeCount);
    }

    //select the best parameters
    vk::SurfaceFormatKHR surfaceFormat = VulkanChooseSwapSurfaceFormat(m_data.SurfaceFormats);
    vk::PresentModeKHR presentMode = VulkanChooseSwapPresentMode(m_data.SurfacePresentModes);
    vk::Extent2D extent = VulkanChooseSwapExtent(m_data.Capabilitites);

    m_data.ImageFormat = surfaceFormat.format;
    m_data.ImageColorSpace = surfaceFormat.colorSpace;
    m_data.PresentMode = presentMode;
    m_data.ImageExtent = extent;

    return TRUE;
}

void Swapchain::ImagesQuery(RendererContext& context)
{
    //cache and retrieving the swap chain data
    m_data.Images.resize(m_data.ImageCount);
    vk::Result result = context.Device.getSwapchainImagesKHR(m_data.Swapchain, &m_data.ImageCount, m_data.Images.data());
    CORE_ASSERT(result == vk::Result::eSuccess, "failed to get swapchain images");

    m_data.CurrentFrame = 0;

    //setup frames in flight

    //Double buffering
    m_data.MaxFramesInFlight = m_data.Images.size() < 2 ? m_data.Images.size() : 2;

    //Triple buffering
    //vkInfo->swapChain.maxFramesInFlight = vkInfo->swapChain.images.impl.count < 3 ? (Uint32)vkInfo->swapChain.images.impl.count : 3;
}

vk::SurfaceFormatKHR Swapchain::VulkanChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
    for (size_t i = 0; i < availableFormats.size(); i++)
    {
        if (availableFormats[i].format == vk::Format::eB8G8R8A8Srgb && availableFormats[i].colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            return availableFormats[i];
        }
    }

    return availableFormats[0];
}

vk::PresentModeKHR Swapchain::VulkanChooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes)
{
    for (size_t i = 0; i < availablePresentModes.size(); i++)
    {
        if (availablePresentModes[i] == vk::PresentModeKHR::eFifo)
        {
            return availablePresentModes[i];
        }
    }

    return vk::PresentModeKHR::eMailbox;
}

vk::Extent2D Swapchain::VulkanChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        return capabilities.currentExtent;
    }
    else
    {
        vk::Extent2D actualExtent (capabilities.currentExtent.width, capabilities.currentExtent.height);

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

}
