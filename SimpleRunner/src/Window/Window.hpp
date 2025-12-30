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

    SR_INLINE void* GetNative()
    {
        return m_native;
    }

    SR_INLINE void SetDataSize(uint32_t width, uint32_t height)
    {
        m_data.Width = width;
        m_data.Height = height;
    }

    SR_INLINE const Data& GetData()
    {
        return m_data;
    }

private:
    void Create();
    void Destroy();

private:
    void* m_native;
    Data m_data;
};

}
