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
        for (auto& colpair : getOutOfBoundsBalls(true))
        {
            //b.get().newKeyframe({{0, 0}, {0, 0}, m_simulationTime});
            auto b{get<std::reference_wrapper<Ball>>(colpair.first)};
            auto& ballPos{b.get().getPositionAtTime(m_simulationTime)};
            auto& ballVel
                { b.get().getLastKeyframeBeforeTime(m_simulationTime).velocity };
            
            Keyframe keyframe{ballPos, ballVel, m_simulationTime};

            switch (get<Direction>(colpair.second))
            {
            case Direction::right:
            case Direction::left:
                Debug::out("hit bottom or top, bouncing off");
                keyframe.velocity = {-ballVel.x(), ballVel.y()};
                break;

            case Direction::up:
            case Direction::down:
                Debug::out("hit left or right, bouncing off");
                keyframe.velocity = {ballVel.x(), -ballVel.y()};
                break;
            default:
                Debug::err("something has gone terribly wrong in the code for"
                    " collisions with bounds");
                break;
            }

            b.get().newKeyframe(keyframe);
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

    int i{0};
    for (i = 0; i < m_maxCollisionIterations; i++)
    {
        bsTimestep *= 0.5;
        
        if (!getOutOfBoundsBalls(false).empty())
        {
            //printf("out of bounds balls found, rewinding\n");
            m_simulationTime -= bsTimestep;
        }
        else if (getOutOfBoundsBalls(true).empty())
        {
            //printf("no touching balls found, forwarding\n");
            m_simulationTime += bsTimestep;
        }
        else if (getOutOfBoundsBalls(true).size() > 1)
        {
            //printf("over one touching balls found, rewinding\n");
            m_simulationTime -= bsTimestep;
        }
        else
        {
            //printf("all good, breaking after %d iterations\n", i);
            break;
        }
    }
    if (i == m_maxCollisionIterations)
    {
        Debug::err("collision time search failed after " 
            + std::to_string(m_maxCollisionIterations) + " tries at physics time "
            + std::to_string(m_simulationTime) + ". consider increasing"
            " collisionPrecision.");
    }
}

std::vector<std::pair<COLLOBJ, COLLOBJ>> Physiker::getOutOfBoundsBalls
    (bool getTouching)
{
    std::vector<std::pair<COLLOBJ, COLLOBJ>> result{};
    if (!m_world.getWorldBounds())
    {
        return result;
    }
    for (auto& b : m_world.getBallsModifiable())
    {
        // effective radius of ball
        double rad {getTouching 
            ? b.getRadius() + b.getRadius() * m_collisionPrecision
            : b.getRadius()};

        // this is the area in which the *center* of the ball can exist
        Rect collisionBounds
            { m_world.getWorldBounds().value().growBy(-rad)};
        
        Eigen::Vector2d pos{ b.getPositionAtTime(m_simulationTime) };

        // if ball not out of bounds, skip it
        if (collisionBounds.contains(pos))
        {
           continue;
        }

        /*printf("ball out of bounds at coords %.*e", DECIMAL_DIG
            , b.getPositionAtTime(m_simulationTime).x());
        printf("; %.*e", DECIMAL_DIG
            , b.getPositionAtTime(m_simulationTime).y());
        printf(" with radius %.*e\n", DECIMAL_DIG, rad);
        fflush(stdout);*/

        Direction dir{};

        // last check is unnecessary, but makes errors easier to catch
        if      (pos.x() >= collisionBounds.getRight()) { dir = Direction::right;}
        else if (pos.x() <= collisionBounds.getLeft())  { dir = Direction::left; }
        else if (pos.y() <= collisionBounds.getTop())   { dir = Direction::up;   }
        else if (pos.y() >= collisionBounds.getBottom()){ dir = Direction::down; }
        
        std::pair<COLLOBJ, COLLOBJ> collisionPair{b, dir};
        
        result.push_back(collisionPair);
    }
    return result;
}