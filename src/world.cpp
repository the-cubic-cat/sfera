#include "world.hpp"

void Ball::newKeyframe(Keyframe keyframe)
{
    m_keyframes.push_back(keyframe);
}

const Vector2d Ball::getPositionAtTime(double time)
{
    Keyframe& lastKey{m_keyframes.back()};

    // starting from last keyframe, searches for first keyframe with time before
    // the specified time
    for (auto& k : std::views::reverse(m_keyframes))
    {
        if (k.keyframeTime < time)
        {
            lastKey = k;
            break;
        }
    }
    return lastKey.startPosition + lastKey.velocity * (time 
        - lastKey.keyframeTime);
}

void World::newBall(double radius, Vector2d position, double mass
, Vector2d velocity)
{
    // vscode incorrectly marks this as error
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

World::World()
    : m_balls{}
    , m_bounds{}
{}