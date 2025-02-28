#include "world.hpp"

using Eigen::Vector2d;

std::string Ball::getTagsAsString() const
{
    std::string r{};
    int i{};
    for (const auto& tag : tags)
    {
        if (i == 0) { r.append(tag); }
        else        { r.append(";" + tag); }
        i++;
    }
    return r;
}
std::deque<std::string> Ball::getTagsFromString(std::string str)
{
    return splitString(str, ';');
}

bool Ball::hasTag(std::string tag) const
{
    if (tag.empty()) { return true; }

    for (auto& t : tags)
    {
        if (t == tag) { return true; }
    }
    return false;
}

bool Ball::isInBounds(Rect bounds, Time time) const
{
    // bounds with radius accounted
    Rect actualBounds { bounds.growBy(-m_radius)};

    if (actualBounds.contains(getPositionAtTime(time)))
    {
        return true;
    }
    return false;
}

double Ball::getKineticEnergy(Time time) const
{
    double speedSquared{getLastKeyframeBeforeTime(time).velocity.squaredNorm()};

    return (m_mass * speedSquared) / 2;
}

void Ball::newKeyframe(Keyframe keyframe)
{
    m_keyframes.push_back(keyframe);
}

void Ball::purgeKeyframes(Keyframe replacement)
{
    // done like this so that keyframe list is never empty to prevent threading
    // errors
    newKeyframe(replacement);
    m_keyframes.erase(m_keyframes.begin(), m_keyframes.end() - 1);
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
    if (m_keyframes.empty())
    {
        Debug::err("keyframe list for ball with id " + std::to_string(m_id)
            + " is empty. something is horribly wrong.");
        throw WorldException::KeyframeListEmpty;
    }

    // starting from last keyframe, searches for first keyframe with time before
    // the specified time
    for (auto& k : std::views::reverse(m_keyframes))
    {
        if (k.keyframeTime <= time)
        {
            return k;
        }
    }
    // Debug::err("time is inaccessible: " + std::to_string(time.getS()));
    // return m_keyframes[0];

    throw WorldException::TimeInaccessible;
}

Ball& World::newBall(double radius, Vector2d position, double mass
    , Vector2d velocity, SDL_Color color, Time time)
{
    Ball ball{radius, position, mass, velocity, color, time};

    if (m_bounds && !ball.isInBounds(m_bounds.value(), time))
    {
        throw WorldException::InvalidBallPosition;
    }
    // a good programmer would unify this check with the one in physics.cpp.
    // i am not a good programmer.
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
    m_balls.push_back(ball);
    return m_balls.back();
}

void World::setWorldBounds(const Rect& bounds, Time time)
{
    for (const auto& b : m_balls)
    {
        if (!b.isInBounds(bounds, time))
        {
            throw WorldException::InvalidBallPosition;
        }
    }
    m_bounds = std::make_optional(bounds);
}

void World::setWorldBounds(const std::optional<Rect>& bounds, Time time)
{
    if (!bounds)
    {
        clearWorldBounds();
        return;
    }
    setWorldBounds(bounds.value(), time);
}

Ball& World::getBallByID(int ID)
{
    auto& ballList{getBallsModifiable()};
    int i{0};
    for (auto& b : ballList)
    {
        if (b.getID() == ID)
        {
            return b;
        }
        i++;
    }
    throw WorldException::BallNotFound;
}

std::vector<std::reference_wrapper<Ball>> World::getBallsWithTag(std::string tag)
{
    std::vector<std::reference_wrapper<Ball>> r{};

    for (auto& b : m_balls)
    {
        if (b.hasTag(tag)) { r.push_back(b); }
    }

    return r;
}