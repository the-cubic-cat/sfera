#include "main.hpp"

int main()
{
    AppState state = AppState::simulation;

    World world{};
    world.newBall(1, {0, 0}, 1, {1, 1});
    //std::thread physicsUpdate;

    Window window{state, world.getBalls()};

    return 0;
}