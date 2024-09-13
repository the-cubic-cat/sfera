#include "main.hpp"

int main()
{
    AppState state = AppState::simulation;

    Debug::out("hello world for a fourth time");

    Window w{state};

    w.waitForClose();

    return 0;
}