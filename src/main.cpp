#include "main.hpp"

int main()
{
    UnitTests::angleInverse();
    UnitTests::vectorRotate();

    AppState state = AppState::simulation;

    World world{};
    world.newBall(1, {-0.5, -3}, 1, {0, 5}, {255, 0, 0, 255});
    world.newBall(1, {0, 3}, 1, {0, -5}, {0, 255, 0, 255});

    //world.newBall(1, {3, 0}, 1, {-1, 0}, {255, 0, 0, 255});
    //world.newBall(1, {-3, 0.3}, 1, {1, 0}, {0, 255, 0, 255});

    //world.newBall(1, {3, 0}, 1, {-1, 0}, {255, 0, 0, 255});
    //world.newBall(1, {-3, -0.3}, 1, {1, 0}, {0, 255, 0, 255});

    world.setWorldBounds({-5, -5, 10, 10});

    Window window{state, world};
    Physiker physiker{state, world};

    std::thread TphysicsUpdate(&Physiker::loop, &physiker);

    window.loop();

    TphysicsUpdate.join();

    return 0;
}