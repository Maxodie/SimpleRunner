#pragma once
#include "Renderer/VulkanInclude.hpp"

namespace SR
{
    class RendererMessageCallback : public nvrhi::IMessageCallback
    {
        virtual void message(nvrhi::MessageSeverity severity, const char* messageText) override;
    };

    struct GraphicsPipelineData
    {
        nvrhi::ShaderHandle VertexShader;
        nvrhi::ShaderHandle FragmentShader;
        nvrhi::InputLayoutHandle InputLayoutHandle;
        nvrhi::GraphicsPipelineHandle GraphicsPipeline;
    };

    struct SwapChainData
    {
        std::vector<nvrhi::TextureHandle> Textures;

        vk::Format ImageFormat;
        vk::PresentModeKHR PresentMode;
        vk::ColorSpaceKHR ImageColorSpace;
        vk::Extent2D ImageExtent;

        vk::CompositeAlphaFlagBitsKHR AlphaFlagBitsKHR = vk::CompositeAlphaFlagBitsKHR::eOpaque;

        vk::SwapchainKHR SwapChain = nullptr;
        uint32_t CurrentSwapChainImageId = 0;

        vk::ImageUsageFlags ImageUsage = vk::ImageUsageFlags::BitsType::eTransferDst | vk::ImageUsageFlags::BitsType::eColorAttachment | vk::ImageUsageFlags::BitsType::eSampled;
        uint32_t ImageCount;

        uint32_t Width, Height;

        vk::SurfaceCapabilitiesKHR Capabilitites;
        std::vector<vk::SurfaceFormatKHR> SurfaceFormats;
        std::vector<vk::PresentModeKHR> SurfacePresentModes;

        std::vector<vk::Semaphore> PresentSemaphores;
        std::vector<vk::Semaphore> AcquireSemaphores;

        uint32_t CurrentFrame = 0;
        uint32_t MaxFramesInFlight;

        std::queue<nvrhi::EventQueryHandle> FramesInFlight;
        std::vector<nvrhi::EventQueryHandle> QueryPool;
    };

    struct RendererContext
    {
        nvrhi::DeviceHandle GetHandle()
        {
            return EnableValidationLayers && ValidationLayer ? ValidationLayer : DeviceHandle;
        }

        nvrhi::vulkan::DeviceHandle& GetVulkanHandle()
        {
            return DeviceHandle;
        }

        void SetHandle(const nvrhi::vulkan::DeviceHandle& consumedHandle)
        {
            DeviceHandle = std::move(consumedHandle);
        }

        void SetValidationHandle(const nvrhi::DeviceHandle& consumedHandle)
        {
            ValidationLayer = std::move(consumedHandle);
        }

    private:
        nvrhi::vulkan::DeviceHandle DeviceHandle = nullptr;
        nvrhi::DeviceHandle ValidationLayer = nullptr;

    public:
        RendererMessageCallback ErrorCallback;
        vk::PhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
        vk::Instance Instance = VK_NULL_HANDLE;
        vk::SurfaceKHR Surface = VK_NULL_HANDLE;
        vk::PhysicalDeviceFeatures PhysicalDeviceFeature;
        vk::PhysicalDeviceProperties PhysicalDeviceProperties;
        vk::PhysicalDeviceMemoryProperties PhysicalDeviceMemoryProperties;
        std::vector<const char*> ValidationLayers;
        std::vector<const char*> PhysicalDeviceExtensions;

        bool SupportsDeviceLocalHostVisible;
        bool EnableValidationLayers = true;
        bool IsContextDeviceInitialized = false;
        char padding[1];
        int GraphicsQueueIndex = 0;

        std::optional<uint32_t> GraphicsFamily = {};
        std::optional<uint32_t> PresentFamily = {};

        vk::Device Device = VK_NULL_HANDLE;
        vk::Queue GraphicsQueue = VK_NULL_HANDLE;
        vk::Queue PresentQueue = VK_NULL_HANDLE;

        std::vector<nvrhi::FramebufferHandle> Framebuffers;
    };
}
