#include "main.hpp"

int main()
{
    AppState state = AppState::simulation;

    World world{};
    world.newBall(1, {0, 0});

    Window window{state, world.getBalls()};

    return 0;
}