#pragma once
#include "Renderer/VulkanStructures.hpp"
#include "Renderer/SwapChain.hpp"
#include "Renderer/GraphicsPipeline.hpp"
#include "Renderer/Buffer.hpp"
#include "Renderer/CommandList.hpp"
#include "nvrhi/nvrhi.h"

namespace SR
{

class RendererAPI
{
public:
    static void Init();
    static void Shutdown();

    SR_INLINE static void ExecuteCommandList(CommandList& commandList)
    {
        GetContext().GetHandle()->executeCommandList(commandList.GetHandle());
    }

    SR_INLINE static RendererContext& GetContext()
    {
        CORE_ASSERT(s_context.IsContextDeviceInitialized, "Renderer API context has not been initialized yet");
        return s_context;
    }

    SR_INLINE static void SetGraphicsState(CommandList& commandList, VertexBuffer& vertexBuffer, IndexBuffer& indexBuffer)
    {
        nvrhi::VertexBufferBinding vertexBufferBinding = nvrhi::VertexBufferBinding()
            .setBuffer(vertexBuffer.GetHandle())
            .setSlot(0)
            .setOffset(0);

        nvrhi::IndexBufferBinding indexBufferBinding = nvrhi::IndexBufferBinding()
            .setBuffer(indexBuffer.GetHandle())
            .setFormat(indexBuffer.GetElementSize() == sizeof(uint32_t) ? nvrhi::Format::R32_UINT : nvrhi::Format::R16_UINT)
            .setOffset(0);

        auto& framebuffer = s_swapChain.GetFrameBufferInFlight();

        auto graphicsState = nvrhi::GraphicsState()
            .setPipeline(s_graphicsPipeline.GetData().GraphicsPipeline)
            .setFramebuffer(framebuffer)
            .setViewport(nvrhi::ViewportState().addViewportAndScissorRect(
                nvrhi::Viewport(s_swapChain.GetData().Width, s_swapChain.GetData().Height))
            )
            // .addBindingSet(bindingSet)
            .addVertexBuffer(vertexBufferBinding)
            .setIndexBuffer(indexBufferBinding);
        commandList.GetHandle()->setGraphicsState(graphicsState);
    }

    static void BeginFrame(CommandList& commandList, const nvrhi::Color& clearColor);
    static void EndFrame(CommandList& commandList);
    static void Present();

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

private:
    static RendererContext s_context;
    static SwapChain s_swapChain;
    static GraphicsPipeline s_graphicsPipeline;
};

}
