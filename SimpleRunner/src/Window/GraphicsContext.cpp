#include "Window/GraphicsContext.hpp"
#include "Log/Log.hpp"
#include <GLFW/glfw3.h>

namespace SR
{

void GraphicsContext::Init()
{
    if(!glfwInit())
    {
        CORE_LOG_ERROR("Failed to initialize glfw");
    }

    CORE_LOG_SUCCESS("glfw graphics context initialized");
}

void GraphicsContext::Shutdown()
{
    glfwTerminate();
    CORE_LOG_SUCCESS("Graphics context has been shutdown");
}

}
