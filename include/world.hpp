#pragma once

#include "appinfo.hpp"
#include "debug.hpp"
#include "rect.hpp"
#include <cstdint>
#include <ranges>

using Eigen::Vector2d;
class Window;

class Keyframe
{
public:
    Vector2d startPosition; // in meters
    Vector2d velocity; // in meters/s
    double keyframeTime; // in seconds
};

class Ball
{
public:
    double getRadius() const { return m_radius; }
    const Vector2d getPositionAtTime(double time) const;
    const Keyframe& getLastKeyframeBeforeTime(double time) const;
    //void draw(const Window& window);
    void newKeyframe(Keyframe keyframe);

private:

    double m_radius; // in meters
    double m_mass; // in kilograms

    std::vector<Keyframe> m_keyframes;

    Ball(double radius, Vector2d position, double mass
    , Vector2d velocity, double time = 0)
        : m_radius{radius}
        , m_mass{mass}
        , m_keyframes{}
    {
        newKeyframe({position, velocity, time});
    }
    friend class World;
};

class World
{
public:
    // creates a new ball, adds it to the ball list
    void newBall(double radius, Vector2d position, double mass = 1
    , Vector2d velocity = {0, 0});

    // set the world bounds
    void setWorldBounds(const Rect& bounds);

    // get const reference to world bounds (if they exist)
    const std::optional<Rect>& getWorldBounds() const;

    // get a const reference to the ball list
    const std::vector<Ball>& getBalls() const;

    // get a modifiable (non-const) reference to the ball list
    std::vector<Ball>& getBallsModifiable();

    World();
    ~World() = default;

    // no copying or moving worlds
    World(const World& world) = delete;
    World& operator=(const World& world) = delete;

    World(World&& world) = delete;
    World& operator=(World&& world) = delete;

private:
    std::vector<Ball> m_balls;
    std::optional<Rect> m_bounds;
};