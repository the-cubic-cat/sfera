#include "main.hpp"

int main()
{
    AppState state = AppState::simulation;

    World world{};
    world.newBall(0.5, {0, 0}, 1, {1, 1});
    world.setWorldBounds({-5, -5, 10, 10});

    Window window{state, world};
    Physiker physiker{state, world};

    std::thread TphysicsUpdate(&Physiker::loop, &physiker);

    window.loop();

    TphysicsUpdate.join();

    return 0;
}