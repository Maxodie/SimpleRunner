#pragma once
#include "Core/Core.hpp"
#include "Event/WindowEvent.hpp"
#include "LayerStack/LayerStack.hpp"
#include "Window/Window.hpp"
#include "Event/Event.hpp"

namespace SR
{

class Application
{
public:
    void Init();
    void Update();
    void Shutdown();

    void OnEvent(Event& event);
    bool OnWindowClosedEvent(const WindowClosedEvent& window);

    void Stop();

    template<typename TLayer, typename... TArgs>
    void AddLayer(TArgs&&... args)
    {
        //post as an event
        layerStack.AddLayer<TLayer>(std::forward(args)...);
    }

    template<typename TLayer>
    void RemoveLayer()
    {
        //post as an event
        layerStack.RemoveLayer<TLayer>();
    }

private:
    LayerStack layerStack;
    bool m_isRunning;

    std::unique_ptr<Window> m_window;
};

}
