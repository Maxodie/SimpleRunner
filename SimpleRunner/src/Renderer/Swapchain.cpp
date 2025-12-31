#include "Renderer/SwapChain.hpp"
#include "Core/Application.hpp"
#include "Log/Log.hpp"

namespace SR
{

void SwapChain::Init(RendererContext& context)
{
    const Window::Data& data = Application::GetApp().GetMainWindow().GetData();
    m_data.Width = data.Width;
    m_data.Height = data.Height;

    InitSwapChainSupportData(context);

    VulkanSwapChainSupportByQuery(context);

    m_data.CurrentFrame = 0;
    m_data.ImageCount = m_data.Capabilitites.minImageCount + 1;

    if (m_data.Capabilitites.maxImageCount > 0 && m_data.ImageCount > m_data.Capabilitites.maxImageCount)// if maxImageCount == 0 then imageCount is illimited
    {
        m_data.ImageCount = m_data.Capabilitites.maxImageCount;
    }

    m_data.MaxFramesInFlight = 2;

    Create(context);
    CreateSemaphores(context);
}

void SwapChain::Shutdown(RendererContext& context)
{
    while(!m_data.FramesInFlight.empty())
    {
        m_data.FramesInFlight.pop();
    }

    m_data.QueryPool.clear();

    Destroy(context);
    DestroySemaphores(context);
    ShutdownSwapChainSupportData();
}

void SwapChain::InitSwapChainSupportData(RendererContext& context)
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

void SwapChain::ShutdownSwapChainSupportData()
{
    m_data.SurfacePresentModes.clear();
    m_data.SurfaceFormats.clear();
}

bool SwapChain::BeginFrame(CommandList& commandList, RendererContext& context)
{
    const auto& semaphore = m_data.AcquireSemaphores[m_data.CurrentFrame];

    vk::Result result;

    int const maxAttempts = 3;
    for (int attempt = 0; attempt < maxAttempts; ++attempt)
    {
        result = context.Device.acquireNextImageKHR(
            m_data.SwapChain,
            (std::numeric_limits<uint64_t>::max)(), // timeout
            semaphore,
            vk::Fence(),
            &m_data.CurrentSwapChainImageId);

        if ((result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) && attempt < maxAttempts)
        {
            auto surfaceCaps = context.PhysicalDevice.getSurfaceCapabilitiesKHR(context.Surface);
            CORE_LOG_WARNING("Failed to render this frame: %d", result);

            m_data.Width = surfaceCaps.currentExtent.width;
            m_data.Height = surfaceCaps.currentExtent.height;

            Recreate(context);
        }
        else
            break;
    }

    m_data.CurrentFrame = (m_data.CurrentFrame + 1) % m_data.MaxFramesInFlight;

    if (result == vk::Result::eSuccess || result == vk::Result::eSuboptimalKHR) // Suboptimal is considered a success
    {
        // Schedule the wait. The actual wait operation will be submitted when the app executes any command list.
        context.GetVulkanHandle()->queueWaitForSemaphore(nvrhi::CommandQueue::Graphics, semaphore, 0);
        return true;
    }

    return false;
}

void SwapChain::EndFrame(RendererContext& context)
{
    const auto& semaphore = m_data.PresentSemaphores[m_data.CurrentSwapChainImageId];

    context.GetVulkanHandle()->queueSignalSemaphore(nvrhi::CommandQueue::Graphics, semaphore, 0);

    // NVRHI buffers the semaphores and signals them when something is submitted to a queue.
    // Call 'executeCommandLists' with no command lists to actually signal the semaphore.
    // equivalent ot the barrier
    context.GetVulkanHandle()->executeCommandLists(nullptr, 0);
}

bool SwapChain::Present(RendererContext& context)
{
    vk::PresentInfoKHR info = vk::PresentInfoKHR()
        .setWaitSemaphoreCount(1)
        .setPWaitSemaphores(&m_data.PresentSemaphores[m_data.CurrentSwapChainImageId])
        .setSwapchainCount(1)
        .setPSwapchains(&m_data.SwapChain)
        .setPImageIndices(&m_data.CurrentSwapChainImageId);

    const vk::Result res = context.PresentQueue.presentKHR(&info);
    if (!(res == vk::Result::eSuccess || res == vk::Result::eErrorOutOfDateKHR || res == vk::Result::eSuboptimalKHR))
    {
        return false;
    }

#ifndef _WIN32
    // if (s_context.VsyncEnabled || s_context.EnableDebugRuntime)
    // {
        // according to vulkan-tutorial.com, "the validation layer implementation expects
        // the application to explicitly synchronize with the GPU"
        // context.PresentQueue.waitIdle();
    // }
#endif

    bool shouldWait = false;
    while (m_data.FramesInFlight.size() >= m_data.MaxFramesInFlight-1)
    {
        auto query = m_data.FramesInFlight.front();
        m_data.FramesInFlight.pop();

        context.GetHandle()->waitEventQuery(query);

        m_data.QueryPool.push_back(query);
    }

    nvrhi::EventQueryHandle query;
    if (!m_data.QueryPool.empty())
    {
        query = m_data.QueryPool.back();
        m_data.QueryPool.pop_back();
    }
    else
    {
        query = context.GetHandle()->createEventQuery();
    }

    context.GetHandle()->resetEventQuery(query);
    context.GetHandle()->setEventQuery(query, nvrhi::CommandQueue::Graphics);
    m_data.FramesInFlight.push(query);

    return true;
}

void SwapChain::Create(RendererContext& context)
{
    CORE_ASSERT(!m_data.SwapChain, "Trying to create a swapChain before destroying the current one");

    m_data.ImageExtent.setWidth(m_data.Width);
    m_data.ImageExtent.setHeight(m_data.Height);

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
    createInfo.oldSwapchain = m_data.SwapChain;

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

    m_data.SwapChain = context.Device.createSwapchainKHR(createInfo);

    auto images = context.Device.getSwapchainImagesKHR(m_data.SwapChain);

    m_data.Textures.resize(images.size());
    for(size_t i = 0; i < images.size(); ++i)
    {
        auto textureDesc = nvrhi::TextureDesc()
            .setDimension(nvrhi::TextureDimension::Texture2D)
            .setFormat(nvrhi::Format::RGBA8_UNORM)
            .setWidth(m_data.Width)
            .setHeight(m_data.Height)
            .setIsRenderTarget(true)
            .setDebugName("Swap Chain Image")
            .setKeepInitialState(true)
            .setInitialState(nvrhi::ResourceStates::Present);

        m_data.Textures[i] = context.GetHandle()->createHandleForNativeTexture(nvrhi::ObjectTypes::VK_Image, nvrhi::Object(images[i]), textureDesc);
    }

    m_data.CurrentSwapChainImageId = 0;

    CORE_LOG_SUCCESS("Vulkan Swapchain created");
}

void SwapChain::Destroy(RendererContext& context)
{
    if (context.Device)
    {
        context.Device.waitIdle();
    }

    if(m_data.SwapChain)
    {
        context.Device.destroySwapchainKHR(m_data.SwapChain);
        m_data.SwapChain = nullptr;
    }

    m_data.Textures.clear();

    CORE_LOG_SUCCESS("Vulkan Swapchain destroyed");
}

void SwapChain::CreateSemaphores(RendererContext& context)
{
    const uint32_t presentSemaphoreCount = m_data.Textures.size();
    m_data.PresentSemaphores.reserve(presentSemaphoreCount);
    for(uint32_t i = 0; i < presentSemaphoreCount; ++i)
    {
        m_data.PresentSemaphores.push_back(context.Device.createSemaphore(vk::SemaphoreCreateInfo()));
    }

    const uint32_t acquireSemaphoreCount = (std::max)(
        static_cast<size_t>(m_data.MaxFramesInFlight),
        m_data.Textures.size()
    );
    for(uint32_t i = 0; i < acquireSemaphoreCount; ++i)
    {
        m_data.AcquireSemaphores.push_back(context.Device.createSemaphore(vk::SemaphoreCreateInfo()));
    }
}

void SwapChain::DestroySemaphores(RendererContext& context)
{
    for(auto& semaphore : m_data.AcquireSemaphores)
    {
        if(semaphore)
        {
            context.Device.destroySemaphore(semaphore);
            semaphore = vk::Semaphore();
        }
    }
    m_data.AcquireSemaphores.clear();

    for(auto& semaphore : m_data.PresentSemaphores)
    {
        if(semaphore)
        {
            context.Device.destroySemaphore(semaphore);
            semaphore = vk::Semaphore();
        }
    }
    m_data.PresentSemaphores.clear();
}

bool SwapChain::VulkanSwapChainSupportByQuery(RendererContext& context)
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

vk::SurfaceFormatKHR SwapChain::VulkanChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
    for (size_t i = 0; i < availableFormats.size(); i++)
    {
        if (availableFormats[i].format == vk::Format::eR8G8B8A8Unorm && availableFormats[i].colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            return availableFormats[i];
        }
    }

    return availableFormats[0];
}

vk::PresentModeKHR SwapChain::VulkanChooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes)
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

vk::Extent2D SwapChain::VulkanChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
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
