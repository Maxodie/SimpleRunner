#pragma once
#include <glm/glm.hpp>
#include <nvrhi/nvrhi.h>

namespace SR
{

struct Vertex
{
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec4 color;

    static constexpr std::array<nvrhi::VertexAttributeDesc, 3> GetAttributes()
    {
        return {
                nvrhi::VertexAttributeDesc()
                    .setName("iPosition")
                    .setFormat(nvrhi::Format::RGB32_FLOAT)
                    .setOffset(offsetof(Vertex, position))
                    .setElementStride(sizeof(Vertex)),
                nvrhi::VertexAttributeDesc()
                    .setName("iTexCoord")
                    .setFormat(nvrhi::Format::RG32_FLOAT)
                    .setOffset(offsetof(Vertex, texCoord))
                    .setElementStride(sizeof(Vertex)),
                nvrhi::VertexAttributeDesc()
                    .setName("iColor")
                    .setFormat(nvrhi::Format::RGBA32_FLOAT)
                    .setOffset(offsetof(Vertex, color))
                    .setElementStride(sizeof(Vertex))
                };
    }
};

}
