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

    SR_INLINE static Application& GetApp()
    {
        if(!s_instance)
        {
            CORE_ASSERT(false, "Trying to get the app before inizializing one");
        }
        return *s_instance;
    }

    SR_INLINE Window& GetMainWindow()
    {
        if(!m_window)
        {
            CORE_ASSERT(false, "Trying to get the main window before the app initialization");
        }

        return *m_window.get();
    }

    void Stop();

    template<typename TLayer, typename... TArgs>
    void AddLayer(TArgs&&... args)
    {
        //post as an event
        m_layerStack.AddLayer<TLayer>(std::forward(args)...);
    }

    template<typename TLayer>
    void RemoveLayer()
    {
        //post as an event
        m_layerStack.RemoveLayer<TLayer>();
    }

private:
    static Application* s_instance;

    LayerStack m_layerStack;
    bool m_isRunning;

    std::unique_ptr<Window> m_window;
};

}
