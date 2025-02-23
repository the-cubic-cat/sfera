#include "debug.hpp"
#include <iostream>

using std::clog;
using std::cout;
using std::cerr;

void print(std::ostream& stream, string_view message)
{
    stream << message << "\n";
}

void Debug::log(string_view message)
{
    //print(clog, message);
}
void Debug::out(string_view message)
{
    print(cout, message);
}
void Debug::err(string_view message)
{
    print(cerr, message);
}