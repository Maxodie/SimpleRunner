#pragma once
#include "Core/Core.hpp"
#include "Event/Event.hpp"
#include <GLFW/glfw3.h>

namespace SR
{

class Window
{
public:
    using Callback = std::function<void(Event&)>;

    struct Data
    {
        uint32_t Width, Height;
        std::string Name;
        Callback callback;
    };

    Window(const Data& data);
    ~Window();

    void PollEvent();

private:
    void Create();
    void Destroy();

private:
    GLFWwindow* m_native;
    Data m_data;
};

}
