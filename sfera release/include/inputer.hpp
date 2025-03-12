#pragma once

#include "world.hpp"
#include <iostream>

// adapted from here: https://stackoverflow.com/questions/15074046/getting-terminal-input-with-sdl-window-open
class Inputer
{
public:
    static bool beginInput();
    static bool hasInput() { return m_state == State::HasInput; }
    static std::string getInput();

    enum class State
    {
        Sleeping,
        Reading,
        HasInput
    };

private:
    static void readInput();

    static State m_state;
    static std::string m_inputString;
};