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
        CommandList CommandList;

        static constexpr Vertex g_Vertices[] =
            {
                //  position          texCoord
                { { -1.f, 0.f, 0.f }, { -1.f, 0.f } },
                { { 1.f, 0.f, 0.f }, { 1.f, 0.f } },
                { { 0.f, 1.f, 0.f }, { 0.f, 1.f } },
                // and so on...
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
