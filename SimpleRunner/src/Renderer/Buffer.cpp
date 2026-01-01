#include "Renderer/Buffer.hpp"
#include "Renderer/VulkanStructures.hpp"

namespace SR
{

void ConstantBuffer::Create(RendererContext& context, size_t size)
{
    auto constantBufferDesc = nvrhi::BufferDesc()
        .setByteSize(size)
        .setIsConstantBuffer(true)
        .setIsVolatile(true)
        .setMaxVersions(16); // number of automatic versions, only necessary on Vulkan

    m_bufferHandle = context.GetHandle()->createBuffer(constantBufferDesc);
}

void VertexBuffer::Create(RendererContext& context, size_t size)
{
    auto vertexBufferDesc = nvrhi::BufferDesc()
        .setByteSize(size)
        .setIsVertexBuffer(true)
        .enableAutomaticStateTracking(nvrhi::ResourceStates::VertexBuffer)
        .setDebugName("Vertex Buffer");

    m_bufferHandle = context.GetHandle()->createBuffer(vertexBufferDesc);

    m_vertexBufferBinding = nvrhi::VertexBufferBinding()
        .setBuffer(GetHandle())
        .setSlot(0)
        .setOffset(0);
}

void IndexBuffer::Create(RendererContext& context, size_t elementSize, size_t count)
{
    m_elementSize = elementSize;
    auto indexBufferDesc = nvrhi::BufferDesc()
        .setByteSize(elementSize * count)
        .setIsIndexBuffer(true)
        .enableAutomaticStateTracking(nvrhi::ResourceStates::IndexBuffer)
        .setDebugName("Index Buffer");

    m_bufferHandle = context.GetHandle()->createBuffer(indexBufferDesc);
    m_indexBufferBinding = nvrhi::IndexBufferBinding()
        .setBuffer(GetHandle())
        .setFormat(GetElementSize() == sizeof(uint32_t) ? nvrhi::Format::R32_UINT : nvrhi::Format::R16_UINT)
        .setOffset(0);
}

}
