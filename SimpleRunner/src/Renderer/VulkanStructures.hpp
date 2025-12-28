#pragma once
#include "Renderer/VulkanInclude.hpp"

namespace SR
{
    class RendererMessageCallback : public nvrhi::IMessageCallback
    {
        virtual void message(nvrhi::MessageSeverity severity, const char* messageText) override;
    };

    struct SwapchainData
    {
        std::vector<vk::Image> Images;
        std::vector<nvrhi::TextureHandle> Textures;

        vk::Format ImageFormat;
        vk::PresentModeKHR PresentMode;
        vk::ColorSpaceKHR ImageColorSpace;
        vk::Extent2D ImageExtent;

        vk::CompositeAlphaFlagBitsKHR AlphaFlagBitsKHR = vk::CompositeAlphaFlagBitsKHR::eOpaque;

        vk::SwapchainKHR Swapchain = nullptr;

        vk::ImageUsageFlags ImageUsage = vk::ImageUsageFlags::BitsType::eColorAttachment | vk::ImageUsageFlags::BitsType::eSampled;
        uint32_t ImageCount;

        uint32_t Width, Height;

        vk::SurfaceCapabilitiesKHR Capabilitites;
        std::vector<vk::SurfaceFormatKHR> SurfaceFormats;
        std::vector<vk::PresentModeKHR> SurfacePresentModes;

        uint32_t CurrentFrame;
        uint32_t MaxFramesInFlight;
    };

    struct RendererContext
    {
        nvrhi::DeviceHandle DeviceHandle;

        RendererMessageCallback ErrorCallback;
        vk::PhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
        vk::Instance Instance = VK_NULL_HANDLE;
        vk::SurfaceKHR Surface = VK_NULL_HANDLE;
        vk::PhysicalDeviceFeatures PhysicalDeviceFeature;
        vk::PhysicalDeviceProperties PhysicalDeviceProperties;
        vk::PhysicalDeviceMemoryProperties PhysicalDeviceMemoryProperties;
        std::vector<const char*> ValidationLayers;

        bool SupportsDeviceLocalHostVisible;
        bool EnableValidationLayers = true;
        char padding[2];
        int GraphicsQueueIndex = 0;

        std::optional<uint32_t> GraphicsFamily = {};
        std::optional<uint32_t> PresentFamily = {};

        vk::Device Device = VK_NULL_HANDLE;
        vk::Queue GraphicsQueue = VK_NULL_HANDLE;
        vk::Queue PresentQueue = VK_NULL_HANDLE;

        nvrhi::FramebufferHandle Framebuffer;
    };
}
