#pragma once

#include "appinfo.hpp"
#include "debug.hpp"
#include "utils.hpp"
#include <cstdint>
#include <ranges>

using Eigen::Vector2d;
class Window;

struct SDL_Color;

class Keyframe
{
public:
    Vector2d startPosition; // in meters
    Vector2d velocity; // in meters/s
    Time keyframeTime;
};

class Ball
{
public:
    double getRadius() const { return m_radius; }
    const Vector2d getPositionAtTime(Time time) const;
    SDL_Color getColor() const { return m_color; }
    const Keyframe& getLastKeyframeBeforeTime(Time time) const;
    const double& getMass() { return m_mass; }
    int getID() const { return m_id; }
    //void draw(const Window& window);
    void newKeyframe(Keyframe keyframe);

private:

    double m_radius; // in meters
    double m_mass; // in kilograms
    SDL_Color m_color;

    std::vector<Keyframe> m_keyframes;

    int m_id; // used to compare balls

    Ball(double radius, Vector2d position, double mass, Vector2d velocity
        , SDL_Color color = {255, 255, 255, 255}, Time time = {})
        : m_radius{radius}
        , m_mass{mass}
        , m_color{color}
        , m_keyframes{}
        , m_id{newBallID()}
    {
        newKeyframe({position, velocity, time});
    }

    int newBallID()
    {
        static int s_ballId{0};
        return s_ballId++;
    }

    friend class World;
};
bool operator== (const Ball& a, const Ball& b);
bool operator!= (const Ball& a, const Ball& b);

class World
{
public:
    // creates a new ball, adds it to the ball list
    void newBall(double radius, Vector2d position, double mass = 1
        , Vector2d velocity = {0, 0}, SDL_Color color = {255, 255, 255, 255}
        , Time time = {});

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