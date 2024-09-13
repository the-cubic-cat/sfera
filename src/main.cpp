#include "main.hpp"

int main()
{
    AppState state = AppState::simulation;

    Debug::out("hello world for a fourth time");

    Window w{};

    while (state == AppState::simulation)
    {
        w.redraw();
    }
    

    return 0;
}