#include "world.hpp"

using Eigen::Vector2d;

void Ball::newKeyframe(Keyframe keyframe)
{
    m_keyframes.push_back(keyframe);
}

bool operator== (const Ball& a, const Ball& b)
{
    return a.getID() == b.getID();
}
bool operator!= (const Ball& a, const Ball& b)
{
    return !operator==(a, b);
}

const Vector2d Ball::getPositionAtTime(Time time) const
{
    auto& k{getLastKeyframeBeforeTime(time)};

    return k.startPosition + k.velocity * (time - k.keyframeTime).getS();
}

const Keyframe& Ball::getLastKeyframeBeforeTime(Time time) const
{
    // starting from last keyframe, searches for first keyframe with time before
    // the specified time
    for (auto& k : std::views::reverse(m_keyframes))
    {
        if (k.keyframeTime <= time)
        {
            return k;
        }
    }

    throw WorldException::TimeInaccessible;
}

void World::newBall(double radius, Vector2d position, double mass
    , Vector2d velocity, SDL_Color color, Time time)
{
    if (m_bounds)
    {
        Rect collisionBounds { m_bounds.value().growBy(-radius)};
        if (!collisionBounds.contains(position))
        {
            throw WorldException::InvalidBallPosition;
        }
    }
    for (auto& b : m_balls)
    {
        Vector2d bPos{b.getPositionAtTime(time)};

        double distanceSquare{((bPos - position).squaredNorm())};
        double collisionDistance{b.getRadius() + radius};
        double collisionDistanceSquare{collisionDistance * collisionDistance};

        if (distanceSquare <= collisionDistanceSquare)
        {
            throw WorldException::InvalidBallPosition;
        }
    }
    m_balls.push_back(Ball{radius, position, mass, velocity, color, time});
}