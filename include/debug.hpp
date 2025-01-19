#pragma once

#include "base.hpp"

using std::string_view;

namespace Debug
{
    void log(string_view message);
    void out(string_view message);
    void err(string_view message);
}