#include "Window/Window.hpp"
#include "Event/WindowEvent.hpp"
#include "GLFW/glfw3.h"

namespace SR
{

Window::Window(const Data& data)
    : m_native(nullptr), m_data(data)
{
    Create();
}

Window::~Window()
{
    Destroy();
}

void Window::PollEvent()
{
    glfwPollEvents();
}

void Window::Create()
{
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    m_native = glfwCreateWindow(m_data.Width, m_data.Height, m_data.Name.c_str(), NULL, NULL);
    if(!m_native)
    {
        CORE_ASSERT(false, "Failed to create glfw window");
    }

    glfwMakeContextCurrent(m_native);
    glfwSetWindowUserPointer(m_native, &m_data);

    glfwSetWindowCloseCallback(
        m_native,
        [](GLFWwindow* window)
        {
            Data* data = static_cast<Data*>(glfwGetWindowUserPointer(window));
            WindowClosedEvent windowClosedEvent{*data};
            data->callback(windowClosedEvent);
        }
    );
    CORE_LOG_SUCCESS("glfw window created");
}

void Window::Destroy()
{
    if(m_native)
    {
        glfwDestroyWindow(m_native);
    }
    CORE_LOG_SUCCESS("glfw window destroyed");
}

}
