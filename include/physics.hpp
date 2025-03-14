#pragma once

#include "inputer.hpp"
#include <fstream>

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
    // used for preliminary collision calculations
    static constexpr double collisionErrorMarginHeuristic{0.1};
    
    // create new physiker
    Physiker(AppState& state, World& world, const Time& currentTime
        , Time timestep = Time::makeMS(2)
        , double collisionErrMargin = 0.0000000001
        , int maxCollisionIterations = 100);

    void setTimestep(Time timestep) { m_timestep = timestep; }
    Time getTimestep() { return m_timestep; }
    Time getSimulationTime() { return m_simulationTime; }

    void setRunaheadTime(Time time) { m_runahead = time; }
    Time getRunaheadTime() { return m_runahead; }

    void setMaxCollisionIterations(int iterations) 
        { m_maxCollisionIterations = iterations; }
    int getMaxCollisionIterations() { return m_maxCollisionIterations; }

    double getKineticEnergy(Time time, std::string_view tag = "");

    void beginLoggingKineticEnergy(const std::string& filename, Time interval
        , const std::deque<std::string>& logTags);
    void stopLoggingKineticEnergy();
    bool isLoggingKineticEnergy() { return m_isLogging; }
    // deletes all keyframes, creates new keyframes with state of balls at 
    // purgeTime, resets simulation time to 0 
    void purgeKeyframes(Time purgeTime);

    // begins executing physics loop. it will run while m_state == simulation.
    void loop();
    
    ~Physiker()
    {
        stopLoggingKineticEnergy();
    }
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
    // tries to find the exact time at which the first collision within the
    // current timestep occurred
    void findCollisionTime();
    // makes balls bounce off walls
    void handleBoundsCollisions();
    // makes balls bounce off each other
    void handleBallCollisions();

    // interval at which physics calculations will be performed
    Time m_timestep;
    // fraction of ball radius that will be used as maximum distance to consider
    // the ball touching
    double m_collisionErrMargin;
    // maximum number of iterations while searching for collision time
    int m_maxCollisionIterations;
    // time currently being evalated in simulation
    Time m_simulationTime;
    // how far ahead of the graphics time physics time is allowed to run
    Time m_runahead;

    // logging stuff
    std::ofstream m_logStream;
    bool m_isLogging;
    Time m_nextLogTime;
    Time m_logInterval;
    std::deque<std::string> m_logTags;
    char m_sep;
    void logKineticEnergy();
    void writeValToLog(std::string_view value);
    // stolen: https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exists-using-standard-c-c11-14-17-c
    bool fileExists(const std::string& name);

    enum struct SearchAction
    {
        None,
        Forward,
        Rewind
    };

    const AppState& m_state;
    World& m_world;
    const Time& m_currentTime;
};