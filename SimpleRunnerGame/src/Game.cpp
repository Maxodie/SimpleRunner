#include "SimpleRunner.hpp"

class GameLayer : public SR::Layer
{
    virtual void OnEvent(SR::Event& event) override
    {
    }

    virtual void Update() override
    {

    }

    virtual void Render() override
    {
        SR::Renderer2D::Begin();
        SR::Renderer2D::DrawQuad(glm::vec2(), glm::vec2());
        SR::Renderer2D::End();
    }
};

void StartApp(SR::Application& app)
{
    app.AddLayer<GameLayer>();
}
