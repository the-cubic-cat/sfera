#include "debug.hpp"
#include <iostream>

using std::clog;
using std::cout;

void print(std::ostream& stream, string_view message)
{
    stream << message << "\n";
}

void Debug::log(string_view message)
{
    print(clog, message);
}
void Debug::out(string_view message)
{
    print(cout, message);
}