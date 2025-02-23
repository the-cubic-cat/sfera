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
    , double collisionErrMargin, int maxCollisionIterations)
    : m_timestep{timestep}
    , collisionErrMargin{collisionErrMargin}
    , m_maxCollisionIterations{maxCollisionIterations}
    , m_simulationTime{}
    , m_state{state}
    , m_world{world}
{}

void Physiker::purgeFuture(Time since)
{
    for (auto& b : m_world.getBallsModifiable())
    {
    
    }
}

void Physiker::loop()
{
    while (m_state == AppState::simulation)
    {
        if (m_simulationTime.getS() > 100) { continue; }
        m_simulationTime += m_timestep;

        findCollisionTime();

        handleBoundsCollisions();
        handleBallCollisions();
    }
}

void Physiker::handleBoundsCollisions()
{
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
            //Debug::log("multiple balls colliding with bounds");
        }
        switch (colpair.getDir())
        {
        case Direction::right:
            keyframe.velocity = {-abs(ballVel.x()), ballVel.y()};
            break;
        case Direction::left:
            keyframe.velocity = {abs(ballVel.x()), ballVel.y()};
            break;
        case Direction::up:
            keyframe.velocity = {ballVel.x(), abs(ballVel.y())};
            break;
        case Direction::down:
            keyframe.velocity = {ballVel.x(), -abs(ballVel.y())};
            break;
        default:
            Debug::err("something has gone terribly wrong in the code for"
                " collisions with bounds");
            break;
        }
        b.newKeyframe(keyframe);
    }
}

void Physiker::handleBallCollisions()
{
    auto list{getCollidingBalls(true)};
    for (auto& colpair : list.get())
    {
        Debug::log("balls collided");

        using Eigen::Vector2d;
        using Eigen::Rotation2Dd;

        Ball& ballA{colpair.getFirst()};
        Ball& ballB{colpair.getSecond()};

        // coords flipped because in sdl2 positive y is down

        // ball centers unflipped
        const Vector2d& Oauf {ballA.getPositionAtTime(m_simulationTime)};
        const Vector2d& Obuf {ballB.getPositionAtTime(m_simulationTime)};

        // ball centers
        const Vector2d Oa {flipVector2d(Oauf, Axis::Y)};
        const Vector2d Ob {flipVector2d(Obuf, Axis::Y)};

        // velocity vectors before collision (unflipped)
        const Vector2d& Vauf
            {ballA.getLastKeyframeBeforeTime(m_simulationTime).velocity};
        const Vector2d& Vbuf
            {ballB.getLastKeyframeBeforeTime(m_simulationTime).velocity};

        // velocity vectors before collision (flipped)
        const Vector2d Va{flipVector2d(Vauf, Axis::Y)};
        const Vector2d Vb{flipVector2d(Vbuf, Axis::Y)};
        
        // ball masses
        const double& Ma{ballA.getMass()};
        const double& Mb{ballB.getMass()};

        // direction from one ball center to another
        const Vector2d dir{Oa - Ob};
        //angle of the line joining the centers of both balls
        Rotation2Dd prpndclr{atan2(dir.y(), dir.x())};
        if (prpndclr.smallestAngle() > PI/2) { prpndclr.angle() -= PI; }
        if (prpndclr.smallestAngle() < -PI/2) { prpndclr.angle() += PI; }

        // rotated velocity vectors before collision
        const Vector2d rotVa{prpndclr.inverse() * Va};
        const Vector2d rotVb{prpndclr.inverse() * Vb};

        // rotatated velocity vectors after collision
        const Vector2d rotVa2
        {
            ((Ma - Mb) * rotVa.x() + 2 * Mb * rotVb.x()) / (Ma + Mb)
            , rotVa.y()
        };
        const Vector2d rotVb2
        {
            rotVa2.x() + rotVa.x() - rotVb.x()
            , rotVb.y()
        };

        // velocity vectors after collision
        const Vector2d Va2{prpndclr * rotVa2};
        const Vector2d Vb2{prpndclr * rotVb2};

        Debug::log("kinetic energy after collision: "
            + std::to_string((Ma * pow(Va2.norm(), 2) + Mb * pow(Vb2.norm(), 2)) / 2));

        ballA.newKeyframe({Oauf, flipVector2d(Va2, Axis::Y), m_simulationTime});
        ballB.newKeyframe({Obuf, flipVector2d(Vb2, Axis::Y), m_simulationTime});
    }
}

void Physiker::findCollisionTime()
{
    if(getOutOfBoundsBalls(false).empty() 
        && getCollidingBalls(false).get().empty())
    {
        return;
    }
    // bs stands for "binary search"
    Time bsTimestep{m_timestep};

    // simulation time at the beginning of search
    Time startSimTime{m_simulationTime};

    SearchAction currentAction{};
    SearchAction lastAction{};

    int i{0};
    for (i = 0; i < m_maxCollisionIterations; i++)
    {
        currentAction = SearchAction::None;
        
        if (!getOutOfBoundsBalls(false).empty() 
            || !getCollidingBalls(false).get().empty())
        {
            Debug::log("rewinding\n");
            currentAction = SearchAction::Rewind;
        }
        else
        {
            Debug::log("forwarding\n");
            currentAction = SearchAction::Forward;
        }

        if (currentAction != lastAction)
        {
            bsTimestep = bsTimestep.getHalf();
            Debug::log("current action != last action\n");
        }

        switch (currentAction)
        {
        case SearchAction::Forward:
            m_simulationTime += bsTimestep;
            break;
        case SearchAction::Rewind:
            m_simulationTime -= bsTimestep;
            break;
        default:
            break;
        }

        Debug::log("time: " + std::to_string(m_simulationTime.getNS()));
        Debug::log(std::to_string(bsTimestep.getNS()));

        // makes sure that time actually moves forward
        if (bsTimestep.getNS() == 1 
            || m_simulationTime < startSimTime - m_timestep.getHalf())
        {
            m_simulationTime += bsTimestep;
            break;
        }

        lastAction = currentAction;
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
        
        // not knowing linear algebra, the fact that norm() returns the absolute
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