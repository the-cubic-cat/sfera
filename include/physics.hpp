#pragma once

#include "system.hpp"

// class handling physics
class Physiker
{
public:
    // create new physiker
    Physiker(AppState& state, World& world, double timestep = 0.02
        , double collisionPrecision = 0.01);

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

    // interval at which physics calculations will be performed
    double m_timestep;
    // fraction of ball radius that will be used as maximum distance to consider
    // the ball touching
    double m_collisionPrecision;
    // time currently being evalated in simulation
    double m_simulationTime;

    const AppState& m_state;
    World& m_world;
};