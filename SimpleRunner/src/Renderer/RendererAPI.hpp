#pragma once
#include "Renderer/Swapchain.hpp"
#include "Renderer/VulkanStructures.hpp"

namespace SR
{

class RendererAPI
{
public:
    static void Init();
    static void Shutdown();

    static const RendererContext& GetContext() { return s_context; }

private:
    static void VulkanInitValidationLayer();
    static bool VulkanCheckValidationLayerSupport();
    static void VulkanPopulateDebugMessenger(vk::DebugUtilsMessengerCreateInfoEXT& createInfo);

    static void LoadQueueFamilies(vk::PhysicalDevice device);
    static bool TryLoadingPhysicalDevice(vk::PhysicalDevice device);
    static vk::PhysicalDevice FindPhysicalDevice();

    static vk::Device CreateDevice();
    static void DestroyDevice();

    static void CreateInstance();
    static void DestroyInstance();

    static void GLFWCreateSurface();
    static void GLFWDestroySurface();

    static void GLFWSetExtension(vk::InstanceCreateInfo* createInfo, std::vector<const char*>& extensionList);

    static void CreateFrameBuffer();
    static void DestroyFrameBuffer();

private:
    static RendererContext s_context;
    static Swapchain s_swapchain;
};

}
