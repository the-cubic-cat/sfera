#include "world.hpp"

void World::newBall(double radius, Vector2d position, double mass
, Vector2d velocity)
{
    m_balls.push_back(Ball{radius, position, mass, velocity});
}

const std::vector<Ball>& World::getBalls()
{
    return m_balls;
}

World::World()
    : m_balls{}
{}