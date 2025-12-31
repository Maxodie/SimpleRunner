#include "Log/Log.hpp"
#include "SimpleRunner.hpp"

class EditorLayer : public SR::Layer
{
public:

    virtual void OnEvent(SR::Event& event) override
    {

    }

    virtual void Update() override
    {

    }

    virtual void Render() override
    {
        SR::Renderer2D::Begin();
        SR::Renderer2D::DrawQuad(glm::vec2(0, 0), glm::vec2(1, 1));
        SR::Renderer2D::End();
    }
};

void StartApp(SR::Application& app)
{
    app.AddLayer<EditorLayer>();
}
