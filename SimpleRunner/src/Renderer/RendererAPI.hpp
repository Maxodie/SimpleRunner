#pragma once
#include "Renderer/Swapchain.hpp"
#include "Renderer/GraphicsPipeline.hpp"
#include "Renderer/Buffer.hpp"
#include "Renderer/CommandList.hpp"
#include "Renderer/VulkanStructures.hpp"

namespace SR
{

class RendererAPI
{
public:
    static void Init();
    static void Shutdown();

    SR_INLINE static void ExecuteCommandList(CommandList& commandList)
    {
        GetContext().DeviceHandle->executeCommandList(commandList.GetHandle());
    }

    SR_INLINE static RendererContext& GetContext()
    {
        CORE_ASSERT(s_context.IsContextDeviceInitialized, "Renderer API context has not been initialized yet");
        return s_context;
    }

    SR_INLINE static void SetGraphicsState(CommandList& commandList, VertexBuffer& vertexBuffer)
    {
        nvrhi::VertexBufferBinding vertexBufferBinding;
        vertexBufferBinding.setBuffer(vertexBuffer.GetHandle());
        vertexBufferBinding.setSlot(0);
        vertexBufferBinding.setOffset(0);

        auto graphicsState = nvrhi::GraphicsState()
            .setPipeline(s_graphicsPipeline.GetData().GraphicsPipeline)
            .setFramebuffer(GetContext().Framebuffer)
            .setViewport(nvrhi::ViewportState().addViewportAndScissorRect(
                nvrhi::Viewport(s_swapchain.GetData().Width, s_swapchain.GetData().Height))
            )
            // .addBindingSet(bindingSet)
            .addVertexBuffer(vertexBufferBinding);
        commandList.GetHandle()->setGraphicsState(graphicsState);
    }

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
    static GraphicsPipeline s_graphicsPipeline;
};

}
