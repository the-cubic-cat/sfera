#include "physics.hpp"

bool operator== (const BallPair& a, const BallPair& b)
{
    if (a.getFirst() == b.getFirst() && a.getSecond() == b.getSecond())
    { return true; }

    return false;
}
bool operator!= (const BallPair& a, const BallPair& b)
{
    return !operator==(a, b);
}

Physiker::Physiker(AppState& state, World& world, Time timestep
    , double collisionErrMargin)
    : m_timestep{timestep}
    , collisionErrMargin{collisionErrMargin}
    , m_maxCollisionIterations{100}
    , m_simulationTime{}
    , m_state{state}
    , m_world{world}
{}

void Physiker::loop()
{
    while (m_state == AppState::simulation)
    {
        if (m_simulationTime.getS() > 500) { continue; }
        m_simulationTime += m_timestep;
        collisionStuff();

        auto collidingBalls{getCollidingBalls(true)};
        for (auto& colpair : collidingBalls.get())
        {
            Debug::log("balls collided");
            colpair.getFirst().newKeyframe({{-1, 0}, {0, 0}, m_simulationTime});
            colpair.getSecond().newKeyframe({{1, 0}, {0, 0}, m_simulationTime});
        }
        for (auto& colpair : getOutOfBoundsBalls(true))
        {
            //b.get().newKeyframe({{0, 0}, {0, 0}, m_simulationTime});
            auto& b{colpair.getBall()};

            auto& ballPos{b.getPositionAtTime(m_simulationTime)};
            auto& ballVel
                { b.getLastKeyframeBeforeTime(m_simulationTime).velocity };
            
            Keyframe keyframe{ballPos, ballVel, m_simulationTime};

            if (getOutOfBoundsBalls(true).size() > 1)
            {
                //keyframe.startPosition = {0, 0};
                //keyframe.velocity = {0, 0};
                Debug::log("more than 1 ball colliding");
            }

            switch (colpair.getDir())
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

            b.newKeyframe(keyframe);
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
    Time bsTimestep{m_timestep};

    int i{0};
    for (i = 0; i < m_maxCollisionIterations; i++)
    {
        bsTimestep = bsTimestep.getHalf();
        Debug::log(std::to_string(bsTimestep.getNS()));
        
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
            + std::to_string(m_maxCollisionIterations) 
            + " tries at physics time " + std::to_string(m_simulationTime.getS()) 
            + ". consider increasing collisionErrMargin.");
    }
}

BallPairVector Physiker::getCollidingBalls(bool getTouching)
{
    std::vector<BallPair> result;
    auto& balls = m_world.getBallsModifiable();

    for (auto& ba : balls){
    for (auto& bb : balls)
    {
        if (bb == ba) { continue; }

        Eigen::Vector2d aPos{ba.getPositionAtTime(m_simulationTime)};
        double aRad{ba.getRadius()};

        Eigen::Vector2d bPos{bb.getPositionAtTime(m_simulationTime)};
        double bRad{bb.getRadius()};
        
        // not knowing vector math, the fact that norm() returns the absolute
        // value of a vector and not the normalized form of a vector severely
        // fucked me over
        double distanceSquare{((aPos - bPos).squaredNorm())};
        double collisionDistance{aRad + bRad};

        if (getTouching) 
            { collisionDistance += std::max(bRad, aRad) * collisionErrMargin; }
        
        double collisionDistanceSquare{collisionDistance * collisionDistance};

        if (distanceSquare <= collisionDistanceSquare)
        {
            result.push_back({ba, bb});
        }
    }}

    return result;
}

std::vector<BoundBallPair> Physiker::getOutOfBoundsBalls(bool getTouching)
{
    std::vector<BoundBallPair> result{};
    if (!m_world.getWorldBounds())
    {
        return result;
    }
    for (auto& b : m_world.getBallsModifiable())
    {
        // effective radius of ball
        double rad {getTouching 
            ? b.getRadius() + b.getRadius() * collisionErrMargin
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
        
        BoundBallPair collisionPair{b, dir};
        
        result.push_back(collisionPair);
    }
    return result;
}