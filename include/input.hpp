#pragma once

#include "physics.hpp"
#include "window.hpp"

enum class InputState
{
    Disabled,
    NoInput,
    HasInput
};

class Inputer
{
public:

private:
    

    static InputState m_state;
    static std::string m_inputString;
};