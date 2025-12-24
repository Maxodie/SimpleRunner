#include "EntryPoint.hpp"
#include "Core/Application.hpp"

int main(int cArgs, char** vArgs)
{
    SR::Application App;
    App.Init();

    StartApp();

    App.Update();

    App.Shutdown();

    return 0;
}
