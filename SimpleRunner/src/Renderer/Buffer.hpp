#pragma once
#include "Renderer/VulkanInclude.hpp"
#include "Renderer/Vertex.hpp"

namespace SR
{

struct RendererContext;

class RendererBuffer
{
public:
    SR_INLINE virtual void Destroy()
    {
        m_bufferHandle = nullptr;
    }

    SR_INLINE nvrhi::BufferHandle& GetHandle()
    {
        CORE_ASSERT(m_bufferHandle, "Trying to get the buffer handle before creating it");
        return m_bufferHandle;
    }

protected:
    nvrhi::BufferHandle m_bufferHandle = nullptr;
};

class ConstantBuffer : public RendererBuffer
{
public:
    void Create(RendererContext& context, size_t size = sizeof(float) * 16);
};

class VertexBuffer : public RendererBuffer
{
public:
    virtual void Create(RendererContext& context, size_t size);

    SR_INLINE nvrhi::VertexBufferBinding& GetBinding()
    {
        return m_vertexBufferBinding;
    }

private:
    nvrhi::VertexBufferBinding m_vertexBufferBinding;
};

class IndexBuffer : public RendererBuffer
{
public:
    void Create(RendererContext& context, size_t elementSize, size_t count);

    SR_INLINE nvrhi::IndexBufferBinding& GetBinding()
    {
        return m_indexBufferBinding;
    }

    SR_INLINE size_t GetElementSize()
    {
        return m_elementSize;
    }

private:
    nvrhi::IndexBufferBinding m_indexBufferBinding;
    size_t m_elementSize;
};

}
