#include "EntryPoint.hpp"
#include "Core/Application.hpp"

int main(int cArgs, char** vArgs)
{
    SR::Application app;
    app.Init();

    StartApp(app);

    app.Update();

    app.Shutdown();
}
