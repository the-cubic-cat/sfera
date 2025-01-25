#pragma once

#include "world.hpp"

class System
{
public:
    // create a new system
    System(AppState& state, const World& world)
        : m_world{world}
        , m_state{state}
    {}
    // destroy the system
    virtual ~System() = default;

    // it doesn't make sense for systems to be copied or moved
    System(const System& system) = delete;
    System& operator=(const System& system) = delete;

    System(System&& system) = delete;
    System& operator=(System&& system) = delete;

protected:
    // system loop
    virtual void loop() = 0;

    // references to world and app state
    const World& m_world;
    AppState& m_state;
};