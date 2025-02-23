#include "inputer.hpp"

Inputer::State Inputer::m_state{State::Sleeping};
std::string Inputer::m_inputString{""};

bool Inputer::beginInput()
{
    if (m_state != State::Sleeping)
    {
        return false;
    }
    m_state = State::Reading;
    std::thread TgetInput{readInput};
    TgetInput.detach();
    return true;
}

std::string Inputer::getInput()
{
    m_state = State::Sleeping;
    return m_inputString;
}

void Inputer::readInput()
{
    std::getline(std::cin, m_inputString);
    m_state = State::HasInput;
}