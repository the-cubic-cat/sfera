#include "world.hpp"

void World::newBall(double radius, Vector2d position, double mass
, Vector2d velocity)
{
    m_balls.push_back(Ball{radius, position, mass, velocity});
}

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

const std::vector<Ball>& World::getBalls()
{
    return m_balls;
}

World::World()
    : m_balls{}
{}