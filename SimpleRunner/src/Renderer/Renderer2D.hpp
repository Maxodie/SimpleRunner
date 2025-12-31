#pragma once
#include "glm/glm.hpp"
#include "Renderer/RendererAPI.hpp"

namespace SR
{

class Renderer2D
{
public:
    struct Data
    {
        VertexBuffer QuadVertexBuffer;
        IndexBuffer QuadIndexBuffer;
        CommandList CommandList;

        static constexpr uint32_t Indices[] =
            {
                0, 1, 2, 2, 3, 0
            };
        static constexpr Vertex Vertices[] =
            {
                //  position          texCoord         color
                { { -0.5f, 0.5f, 0.f }, { 0.f, 0.f }, {1.f, 0, 0, 1.f} },
                { { 0.5f, 0.5f, 0.f }, { 1.f, 0.f }, {0.f, 1.0f, 0, 1.f} },
                { { 0.5f, -0.5f, 0.f }, { 1.f, 1.f }, {0.f, 0, 1.0f, 1.f} },
                { { -0.5f, -0.5f, 0.f }, { 0.f, 1.f }, {1.0f, 1.0f, 1.0f, 1.f} },
            };
    };

    static void Init();
    static void Shutdown();

    static void Begin();
    static void End();

    static void DrawQuad(glm::vec2 position, glm::vec2 scale);

private:
    static Data s_data;
};

}
