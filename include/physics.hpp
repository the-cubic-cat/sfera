#pragma once

#include "system.hpp"
#include <variant>

// collision object колобжок)))
#define COLLOBJ std::variant<Direction, std::reference_wrapper<Ball>>

enum class Direction
{
    none,
    up,
    down,
    left,
    right
};

// class handling physics
class Physiker
{
public:
    // create new physiker
    Physiker(AppState& state, World& world, double timestep = 0.002
        , double collisionPrecision = 0.000000000000001);

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
    std::vector<std::reference_wrapper<Ball>> getOutOfBoundsBalls(bool getTouching);
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