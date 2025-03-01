#pragma once

#include "appinfo.hpp"
#include "debug.hpp"
#include "utils.hpp"
#include <cstdint>
#include <ranges>

class Window;

struct SDL_Color;

class Keyframe
{
public:
    Eigen::Vector2d startPosition{0, 0}; // in meters
    Eigen::Vector2d velocity{0, 0}; // in meters/s
    Time keyframeTime{};
};

class Ball
{
public:
    std::deque<std::string> tags;

    // get tags as one semicolon-separated string
    std::string getTagsAsString() const;
    // get tags from one semicolon-separated string
    static std::deque<std::string> getTagsFromString(std::string_view str);
    // always returns true with empty string as input
    bool hasTag(std::string_view tag) const;

    bool isInBounds(Rect bounds, Time time) const;

    double getRadius() const { return m_radius; }
    const Eigen::Vector2d getPositionAtTime(Time time) const;
    SDL_Color getColor() const { return m_color; }
    const Keyframe& getLastKeyframeBeforeTime(Time time) const;
    const double& getMass() const { return m_mass; }
    int getID() const { return m_id; }

    double getKineticEnergy(Time time) const;
    //void draw(const Window& window);

    // creates new keyframe
    void newKeyframe(Keyframe keyframe);
    // deletes all keyframes and replaces them with the one given
    // USE CAREFULLY, CAN BREAK THINGS
    void purgeKeyframes(Keyframe replacement);

private:

    double m_radius; // in meters
    double m_mass; // in kilograms
    SDL_Color m_color;

    std::vector<Keyframe> m_keyframes;

    int m_id; // used to compare balls

    Ball(double radius, Eigen::Vector2d position, double mass, Eigen::Vector2d velocity
        , SDL_Color color = {255, 255, 255, 255}, Time time = {}
        , const std::deque<std::string>& tags = {})
        : tags{tags}
        , m_radius{radius}
        , m_mass{mass}
        , m_color{color}
        , m_keyframes{{position, velocity, time}}
        , m_id{newBallID()}
    {}

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
    Ball& newBall(double radius, Eigen::Vector2d position, double mass = 1
        , Eigen::Vector2d velocity = {0, 0}, SDL_Color color = {255, 255, 255, 255}
        , Time time = {});

    // set the world bounds
    void setWorldBounds(const std::optional<Rect>& bounds, Time time);
    void clearWorldBounds() { m_bounds = std::nullopt; }
    void setWorldBounds(const Rect& bounds, Time time);

    // get const reference to world bounds (if they exist)
    const std::optional<Rect>& getWorldBounds() const { return m_bounds; }

    // get a const reference to the ball list
    const std::vector<Ball>& getBalls() const { return m_balls; };

    // get a modifiable (non-const) reference to the ball list
    std::vector<Ball>& getBallsModifiable() { return m_balls; };
    // get a non-const reference to a ball with the given ID
    Ball& getBallByID(int ID);
    // get all balls with specified tag
    std::vector<std::reference_wrapper<Ball>> getBallsWithTag(std::string_view tag);

    World() : m_balls{}, m_bounds{} {}
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

enum class WorldException
{
    TimeInaccessible,
    InvalidBallPosition,
    BallNotFound,
    KeyframeListEmpty
};