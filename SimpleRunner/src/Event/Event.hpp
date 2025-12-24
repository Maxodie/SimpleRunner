#pragma once
#include "Core/Core.hpp"

namespace SR
{

enum class EventType : uint8_t
{
    CloseWindow, ResizeWindow, MinimizeWindow
};

#define EVENT_CLASS_TYPE(type) SR_INLINE static EventType GetStaticType() { return EventType::type; }\
                                SR_INLINE virtual EventType GetType() const override { return GetStaticType(); }

class Event
{
public:
    bool Handled = false;

    SR_INLINE virtual EventType GetType() const = 0;
};

class EventDispatcher
{
public:
    EventDispatcher(Event& event)
        : m_event(event) {}

    template<typename TEvent = Event>
    SR_INLINE void Dispatch(const std::function<bool(const TEvent&)>& callback)
    {
        if(m_event.GetType() == TEvent::GetStaticType())
        {
            m_event.Handled = callback(static_cast<TEvent&>(m_event));
        }
    }

private:
    Event& m_event;
};

}
