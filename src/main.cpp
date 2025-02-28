#include "main.hpp"

int main()
{
    AppState state = AppState::simulation;
    Time currentTime{};

    World world{};

    world.setWorldBounds(Rect{-5, -5, 10, 10}, currentTime);

    Window window{state, world, currentTime};
    Physiker physiker{state, world, currentTime};

    InputHandler::init(world, window, physiker, state);

    std::thread TphysicsUpdate(&Physiker::loop, &physiker);

    window.loop();

    TphysicsUpdate.join();

    return 0;
}