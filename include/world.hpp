#pragma once

#include "appinfo.hpp"
#include "debug.hpp"
#include <cstdint>
#include <ranges>

using Eigen::Vector2d;
class Window;

struct Color
{
    std::uint8_t r{0};
    std::uint8_t g{0};
    std::uint8_t b{0};
};

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
    double getRadius() { return m_radius; }
    const Vector2d getPositionAtTime(double time);
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

    // get a const reference to the ball list
    const std::vector<Ball>& getBalls();

    World();
    ~World() = default;

    // no copying or moving worlds
    World(const World& world) = delete;
    World& operator=(const World& world) = delete;

    World(World&& world) = delete;
    World& operator=(World&& world) = delete;

private:
    std::vector<Ball> m_balls;
};