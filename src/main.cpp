#include "main.hpp"

int main()
{
    AppState state = AppState::simulation;

    World world{};
    world.newBall(0.5, {-3, 0}, 1, {0.7, 2.8}, {255, 0, 0, 255});
    world.newBall(0.5, {3, 0}, 1, {-1.76, 2}, {0, 255, 0, 255});
    world.setWorldBounds({-5, -5, 10, 10});

    Window window{state, world};
    Physiker physiker{state, world};

    std::thread TphysicsUpdate(&Physiker::loop, &physiker);

    Eigen::Vector2d test{2, 1};

    Debug::out(std::to_string(test.norm()));

    window.loop();

    TphysicsUpdate.join();

    return 0;
}