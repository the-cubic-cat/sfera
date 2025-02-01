#include "world.hpp"

void Ball::newKeyframe(Keyframe keyframe)
{
    m_keyframes.push_back(keyframe);
    Debug::out("new keyframe");
}

const Vector2d Ball::getPositionAtTime(double time) const
{
    // starting from last keyframe, searches for first keyframe with time before
    // the specified time
    for (auto& k : std::views::reverse(m_keyframes))
    {
        if (k.keyframeTime < time)
        {
            return k.startPosition + k.velocity * (time - k.keyframeTime);
        }
    }
    Debug::err("time is inaccessible: " + std::to_string(time));
    return {0, 0};
}

void World::newBall(double radius, Vector2d position, double mass
, Vector2d velocity)
{
    m_balls.push_back(Ball{radius, position, mass, velocity});
}

void World::setWorldBounds(const Rect& bounds)
{
    m_bounds = bounds;
}

const std::optional<Rect>& World::getWorldBounds() const
{
    return m_bounds;
}

const std::vector<Ball>& World::getBalls() const
{
    return m_balls;
}

std::vector<Ball>& World::getBallsModifiable()
{
    return m_balls;
}

World::World()
    : m_balls{}
    , m_bounds{}
{}