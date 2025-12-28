#include "Core/Application.hpp"
#include "Core/Core.hpp"
#include "Event/Event.hpp"
#include "Event/WindowEvent.hpp"
#include "Window/GraphicsContext.hpp"
#include "Window/Window.hpp"
#include "Renderer/RendererAPI.hpp"

namespace SR
{

Application* Application::s_instance;

void Application::Init()
{
    if(s_instance)
    {
        CORE_ASSERT(false, "Trying to create a second app. Only one app must exists");
        return;
    }

    s_instance = this;

    GraphicsContext::Init();

    //creation of the main window must occure before the renderer initialization
    m_window = std::make_unique<Window>((Window::Data)
        {.Width=1280, .Height=720, .Name="Game", .callback=BIND_FUNCTION_1(Application::OnEvent)}
    );

    RendererAPI::Init();

    m_isRunning = true;
    CORE_LOG_SUCCESS("Application initialized");
}

void Application::Update()
{
    while(m_isRunning)
    {
        m_window->PollEvent();
    }
}

void Application::Shutdown()
{
    m_window = nullptr;

    RendererAPI::Shutdown();

    GraphicsContext::Shutdown();
    CORE_LOG_SUCCESS("Application has been shutdown");
}

void Application::OnEvent(Event& event)
{
    EventDispatcher dispatcher{event};
    dispatcher.Dispatch<WindowClosedEvent>(BIND_FUNCTION_1(Application::OnWindowClosedEvent));

    for(auto& layer : m_layerStack.GetStack())
    {
        if(!event.Handled)
        {
            layer->OnEvent(event);
        }
    }
}

bool Application::OnWindowClosedEvent(const WindowClosedEvent& window)
{
    m_window = nullptr;

    Stop();
    return true;
}

void Application::Stop()
{
    m_isRunning = false;
    CORE_LOG_SUCCESS("Application stop called");
}
}
