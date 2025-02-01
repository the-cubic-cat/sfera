#include "physics.hpp"

Physiker::Physiker(AppState& state, World& world, double timestep
    , double collisionPrecision)
    : m_timestep{timestep}
    , m_collisionPrecision{collisionPrecision}
    , m_maxCollisionIterations{100}
    , m_simulationTime{0}
    , m_state{state}
    , m_world{world}
{}

void Physiker::loop()
{
    while (m_state == AppState::simulation)
    {
        m_simulationTime += m_timestep;
        collisionStuff();
        for (auto& b : getOutOfBoundsBalls(true))
        {
            b.get().newKeyframe({{0, 0}, {0, 0}, m_simulationTime});
        }
    }
}

void Physiker::collisionStuff()
{
    if(getOutOfBoundsBalls(false).empty())
    {
        return;
    }
    // bs stands for "binary search"
    double bsTimestep{m_timestep};

    for (int i = 0; i < m_maxCollisionIterations; i++)
    {
        bsTimestep *= 0.5;
        
        if (!getOutOfBoundsBalls(false).empty())
        {
            printf("out of bounds balls found, rewinding\n");
            m_simulationTime -= bsTimestep;
        }
        else if (getOutOfBoundsBalls(true).empty())
        {
            printf("no touching balls found, forwarding\n");
            m_simulationTime += bsTimestep;
        }
        else if (getOutOfBoundsBalls(true).size() > 1)
        {
            printf("over one touching balls found, rewinding\n");
            m_simulationTime -= bsTimestep;
        }
        else
        {
            printf("all good, breaking after %d iterations\n", i);
            break;
        }
    }
}

std::vector<std::reference_wrapper<Ball>> Physiker::getOutOfBoundsBalls
    (bool getTouching)
{
    std::vector<std::reference_wrapper<Ball>> result{};
    if (!m_world.getWorldBounds())
    {
        return result;
    }
    for (auto& b : m_world.getBallsModifiable())
    {
        double rad {getTouching 
            ? b.getRadius() + b.getRadius() * m_collisionPrecision
            : b.getRadius()};

        // this is the area in which the *center* of the ball can exist
        Rect ballCollisionBounds
            { m_world.getWorldBounds().value().growBy(-rad)};
        
        if (!ballCollisionBounds.contains(b.getPositionAtTime(m_simulationTime)))
        {
            /*Debug::out("ball out of bounds at coords " 
                + std::to_string(b.getPositionAtTime(m_simulationTime).x()) + 
                "; " + std::to_string(
                b.getPositionAtTime(m_simulationTime).y()));*/
            printf("ball out of bounds at coords %.*e", DECIMAL_DIG
                , b.getPositionAtTime(m_simulationTime).x());
            printf("; %.*e", DECIMAL_DIG
                , b.getPositionAtTime(m_simulationTime).y());
            printf(" with radius %.*e\n", DECIMAL_DIG, rad);
            fflush(stdout);
            
            result.push_back(b);
        }
    }
    return result;
}