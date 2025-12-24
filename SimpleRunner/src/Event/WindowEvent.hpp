#pragma once
#include "Window/Window.hpp"
#include <Event/Event.hpp>

namespace SR
{

class WindowClosedEvent : public Event
{
public:
    WindowClosedEvent(const Window::Data& windowData)
        : WindowData(windowData) {}
    EVENT_CLASS_TYPE(CloseWindow);

    const Window::Data& WindowData;
};

class WindowResizeEvent : public Event
{
public:
    WindowResizeEvent() = default;
    EVENT_CLASS_TYPE(ResizeWindow)
};

class WindowMinimizedEvent : public Event
{
public:
    WindowMinimizedEvent() = default;
    EVENT_CLASS_TYPE(MinimizeWindow);
};

}
