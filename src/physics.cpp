#include "physics.hpp"

Physiker::Physiker(AppState& state, World& world, double timestep
    , double collisionPrecision)
    : m_timestep{timestep}
    , m_collisionPrecision{collisionPrecision}
    , m_simulationTime{0}
    , m_state{state}
    , m_world{world}
{}

void Physiker::loop()
{
    while (m_state == AppState::simulation)
    {
        m_simulationTime += m_timestep;
        for (auto& b : getOutOfBoundsBalls(false))
        {
            b.get().newKeyframe({{0, 0}, {0, 0}, m_simulationTime});
        }
    }
}

std::vector<std::reference_wrapper<Ball>> Physiker::getOutOfBoundsBalls
    (bool getTouching)
{
    std::vector<std::reference_wrapper<Ball>> r{};
    if (!m_world.getWorldBounds())
    {
        return r;
    }
    for (auto& b : m_world.getBallsModifiable())
    {
        // this is the area in which the *center* of the ball can exist
        Rect ballCollisionBounds
            { m_world.getWorldBounds().value().growBy(-b.getRadius())};
        
        if (!)
        
        if (!ballCollisionBounds.contains(b.getPositionAtTime(m_simulationTime)))
        {
            Debug::out("ball out of bounds at coords " 
                + std::to_string(b.getPositionAtTime(m_simulationTime).x()) + 
                "; " + std::to_string(
                b.getPositionAtTime(m_simulationTime).y()));
            r.push_back(b);
        }
    }
    return r;
}