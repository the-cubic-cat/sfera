#pragma once

#include "system.hpp"
#include <variant>

/* collision object колобжок)))
#define COLLOBJ std::variant<Direction, std::reference_wrapper<Ball>>
#define COLLPAIR std::pair<COLLOBJ, COLLOBJ>*/

class BoundBallPair
{
public:
    BoundBallPair(Ball& ball, Direction direction)
        : m_ball{ball}
        , m_dir{direction}
    {}

    Ball& getBall() const { return m_ball; }
    Direction getDir() const { return m_dir; }

    /*BoundBallPair(const BoundBallPair& b) = default;
    BoundBallPair& operator=(const BoundBallPair& b) = default;
    BoundBallPair(const BoundBallPair&& b) = default;*/

private:
    std::reference_wrapper<Ball> m_ball;
    Direction m_dir;
};

class BallPair
{
public:
    // REALLY FUCKING IMPORTANT!!! id of first ball is ALWAYS lower than of second
    BallPair(Ball& ballA, Ball& ballB)
        : m_first{ballA.getID() < ballB.getID() ? ballA : ballB}
        , m_second{ballA.getID() < ballB.getID() ? ballB : ballA}
    {}

    Ball& getFirst() const { return m_first; }
    Ball& getSecond() const { return m_second; }

    friend struct std::hash<BallPair>;
private:
    std::reference_wrapper<Ball> m_first;
    std::reference_wrapper<Ball> m_second;
};
bool operator== (const BallPair& a, const BallPair& b);
bool operator!= (const BallPair& a, const BallPair& b);

template <>
struct std::hash<BallPair>
{
    std::size_t operator()(const BallPair& bp) const
    {
        // zero fucking clue what this does, idk shit about bitshifting
        // stolen: https://stackoverflow.com/questions/17016175/c-unordered-map-using-a-custom-class-type-as-the-key
        return ((hash<int>()(bp.m_first.get().getID())
             ^ (hash<int>()(bp.m_second.get().getID()) << 1)) >> 1);
    }
};

// certified no duplicates
class BallPairVector
{
public:
    BallPairVector(std::vector<BallPair> vector)
        : m_vect{vectRemoveDuplicates(vector)}
    {}

    const std::vector<BallPair>& get() { return m_vect; }

private:
    std::vector<BallPair> m_vect;
};

// class handling physics
class Physiker
{
public:
    // create new physiker
    Physiker(AppState& state, World& world, double timestep = 0.002
        , double collisionPrecision = 0.000000001);

    void setTimestep(double timestep) { m_timestep = timestep; }
    double getTimestep() { return m_timestep; }

    // begins executing physics loop. it will run while m_state == simulation.
    void loop();
    
    Physiker(const Physiker& physiker) = delete;
    Physiker& operator=(const Physiker& physiker) = delete;

    Physiker(Physiker&& physiker) = delete;
    Physiker& operator=(Physiker&& physiker) = delete;
private:
    // return vector of balls out of bounds
    // get touching: if false, return only if the ball is outside the bounds 
    // (that is, its radius overlaps with bounds). if true, return ball if
    // it is touching bounds (that is, radius + m_collisionPrecision 
    // * radius overlaps with bounds)
    std::vector<BoundBallPair> getOutOfBoundsBalls(bool getTouching);

    BallPairVector getCollidingBalls(bool getTouching);
    // rename and split up later
    void collisionStuff();

    // interval at which physics calculations will be performed
    double m_timestep;
    // fraction of ball radius that will be used as maximum distance to consider
    // the ball touching
    double m_collisionPrecision;
    // maximum number of iterations while searching for collision time
    int m_maxCollisionIterations;
    // time currently being evalated in simulation
    double m_simulationTime;

    const AppState& m_state;
    World& m_world;
};