#include "input_handler.hpp"

using std::string;
using std::queue;

bool InputHandler::m_initalized{false};
string InputHandler::helpText
{
    "Your command was not found. Available commands are:\n"
    "graphics\n"
};

std::optional<std::reference_wrapper<World>>    InputHandler::m_world{};
std::optional<std::reference_wrapper<Window>>   InputHandler::m_window{};
std::optional<std::reference_wrapper<Physiker>> InputHandler::m_phys{};

void InputHandler::init(World& world, Window& window, Physiker& physiker)
{
    m_world = world;
    m_window = window;
    m_phys = physiker;

    m_initalized = true;
}

void InputHandler::parseInput(string inputCommand)
{
    if (!m_initalized) { throw "InputHandler not initialized"; }

    makeLowercase(inputCommand);

    queue<string> command{splitString(inputCommand, ' ')};

    try
    {
    string front{dequeue(command)};
    if      (front == "graphics") { graphics::parse(command); }
    else if (front == "world"   ) { world::parse(command);    }
    else { Debug::err("Invalid command"); }
    }
    catch (CommandException ex)
    {
        Debug::err("Something is wrong with your command :(");
    }
}

queue<string> InputHandler::splitString(string str, char separator)
{
    queue<string> r{};

    std::stringstream words(str);
    string tempWord;

    while (getline(words, tempWord, separator))
    {
        r.push(tempWord);
    }

    return r;
}

Time InputHandler::makeTime(string timeString)
{
    Time newTime{};

    try
    {
    if(unsuffix(timeString, "ns"))
    {
        newTime = Time::makeNS(std::stoi(timeString));
    }
    else if (unsuffix(timeString, "ms"))
    {
        newTime = Time::makeMS(std::stoi(timeString));
    }
    else
    {
        unsuffix(timeString, "s");
        newTime = Time::makeS(std::stod(timeString));
    }
    }
    catch (std::invalid_argument const&)
    {
        throw CommandException::NumberExpected;
    }

    return newTime;
}
Eigen::Vector2d InputHandler::makeVector(std::string vectorString)
{
    queue<string> coords{splitString(vectorString, ';')};
    if (coords.size() != 2) { throw CommandException::WrongParameter; }

    double x{std::stod(dequeue(coords))};
    double y{std::stod(dequeue(coords))};

    return {x, y};
}
SDL_Color InputHandler::makeColor(std::string colorString)
{
    queue<string> values{splitString(colorString, ';')};
    if (3 > values.size() || values.size() > 4) 
        { throw CommandException::WrongParameter; }

    int r{std::stoi(dequeue(values))};
    int g{std::stoi(dequeue(values))};
    int b{std::stoi(dequeue(values))};
    int a{255};
    if (!values.empty()) { a = std::stoi(dequeue(values)); }

    if (0 > r || r > 255
     || 0 > g || g > 255
     || 0 > b || b > 255
     || 0 > a || a > 255)
    {
        throw CommandException::WrongParameter;
    }

    return {static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b)
        , static_cast<Uint8>(a)};
}

void InputHandler::graphics::parse(queue<string> command)
{
    string front{dequeue(command)};

    if (front == "time") { time::parse(command); }
    else { throw CommandException::WrongArgument; }
}

void InputHandler::graphics::time::parse(queue<string> command)
{
    string front{dequeue(command)};

    if      (front == "set"   ) { set          (command); }
    else if (front == "move"  ) { move         (command); }
    else if (front == "get"   ) { get          (command); }
    else if (front == "scale" ) { scale::parse (command); }
    else { throw CommandException::WrongArgument; }
}
void InputHandler::graphics::time::set(queue<string> command)
{
    string t{command.front()};

    WINDOW.setTime(makeTime(t));
}
void InputHandler::graphics::time::move(queue<string> command)
{
    string t{command.front()};

    WINDOW.setTime(WINDOW.getTime() + makeTime(t));
}
void InputHandler::graphics::time::get(queue<string> command)
{
    string unit{}; 
    if (!command.empty())
    {
        unit = dequeue(command);
    }

    if (unit == "ns")
    {
        Debug::out(std::to_string(WINDOW.getTime().getNS()) + " nanoseconds");
    }
    else if (unit == "ms")
    {
        Debug::out(std::to_string(WINDOW.getTime().getMS()) + " milliseconds");
    }
    else
    {
        Debug::out(std::to_string(WINDOW.getTime().getS()) + " seconds");
    }
}

void InputHandler::graphics::time::scale::parse(queue<string> command)
{
    string front{dequeue(command)};

    if      (front == "set") { set(command); }
    else if (front == "get") { get();        }
    else { throw CommandException::WrongArgument; }
}
void InputHandler::graphics::time::scale::set(queue<string> command)
{
    string front{dequeue(command)};

    try
    {
        WINDOW.setTimescale(std::stod(front));
    }
    catch (std::invalid_argument const&)
    {
        throw CommandException::NumberExpected;
    }
}
void InputHandler::graphics::time::scale::get()
{
    Debug::out(std::to_string(WINDOW.getTimescale()));
}

void InputHandler::world::parse(queue<string> command)
{
    string front{dequeue(command)};

    if (front == "balls") { balls::parse(command); }
    else { throw CommandException::WrongArgument; }
}

void InputHandler::world::balls::parse(queue<string> command)
{
    string front{dequeue(command)};

    if      (front == "get") { get();            }
    else if (front == "new") { newball(command); }
    else { throw CommandException::WrongArgument; }
}
void InputHandler::world::balls::get()
{
    for (const auto& b : WORLD.getBalls())
    {
        const Eigen::Vector2d& position{b.getPositionAtTime(WINDOW.getTime())};
        const Eigen::Vector2d& velocity{b.getLastKeyframeBeforeTime
            (WINDOW.getTime()).velocity};

        string ID        {std::to_string(b.getID())     };
        string radius    {std::to_string(b.getRadius()) };
        string mass      {std::to_string(b.getMass())   };

        string positionX {std::to_string(position.x())  };
        string positionY {std::to_string(position.y())  };
 
        string velocityX {std::to_string(velocity.x())  };
        string velocityY {std::to_string(velocity.y())  };
        
        string cR        {std::to_string(b.getColor().r)};
        string cG        {std::to_string(b.getColor().g)};
        string cB        {std::to_string(b.getColor().b)};

        Debug::out("ID: " + addSpacing(ID, 8) + "radius: " 
            + addSpacing(radius, 16) + "mass: " + addSpacing(mass, 16)
            + "position: " + addSpacing(positionX + "; " + positionY, 24) 
            + "velocity: " + addSpacing(velocityX + "; " + velocityY, 24) 
            + "color: " + cR + "; " + cG + "; " + cB + "\n");
    }
}

void InputHandler::world::balls::newball(queue<string> command)
{
    double radius{1};
    Eigen::Vector2d position{0, 0};
    double mass{1};
    Eigen::Vector2d velocity{0, 0};
    SDL_Color color{255, 255, 255, 255};
    Time time{WINDOW.getTime()};

    while (!command.empty())
    {
        string param{dequeue(command)};
        try
        {
        if      (unprefix(param, "radius="))  { radius   = std::stod (param); }
        else if (unprefix(param, "mass="))    { mass     = std::stod (param); }
        else if (unprefix(param, "position=")){ position = makeVector(param); }
        else if (unprefix(param, "velocity=")){ velocity = makeVector(param); }
        else if (unprefix(param, "color="))   { color    = makeColor (param); }
        }
        catch (std::invalid_argument const&)
        {
            throw CommandException::NumberExpected;
        }
    }
    try   { WORLD.newBall(radius, position, mass, velocity, color, time); }
    catch (WorldException ex)
    {
        Debug::err("You are trying to create a ball in an invalid position,"
            " such as inside another ball or outside the world bounds.");
    }
}

string makeLowercase(string str)
{
    std::transform(str.begin(), str.end(), str.begin(),
    [](unsigned char c){ return std::tolower(c); });
    
    return str;
}

string addSpacing(string str, int desiredLength)
{
    if (static_cast<int>(str.length()) >= desiredLength) 
        { return str.append(" "); }

    int numSpaces{desiredLength - static_cast<int>(str.length())};

    for (int i{0}; i < numSpaces; i++)
    {
        str.append(" ");
    }

    return str;
}