#pragma once

#include "appinfo.hpp"
#include "debug.hpp"

using Eigen::Vector2d;

class Ball
{
public:
    double getRadius() { return m_radius; }
    const Vector2d& getCurrentPosition() { return m_inputPosition; }

private:
    double m_radius; // in meters
    Vector2d m_inputPosition; // in meters

    double m_mass; // in kilograms  
    Vector2d m_velocity; // in meters/s

    Vector2d m_currentPosition; // in meters

    Ball(double radius, Vector2d position, double mass
    , Vector2d velocity)
        : m_radius{radius}
        , m_inputPosition{position}
        , m_mass{mass}
        , m_velocity{velocity}
    {}

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