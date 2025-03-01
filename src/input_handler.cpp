#include "input_handler.hpp"

using std::string;
using std::queue;

bool InputHandler::m_initalized{false};
string InputHandler::m_helpText
{
    "Your command was not found. Available commands are:\n"
    "graphics\n"
};

std::optional<std::reference_wrapper<World>>    InputHandler::m_world{};
std::optional<std::reference_wrapper<Window>>   InputHandler::m_window{};
std::optional<std::reference_wrapper<Physiker>> InputHandler::m_phys{};
std::optional<std::reference_wrapper<AppState>> InputHandler::m_state{};

void InputHandler::init(World& world, Window& window, Physiker& physiker
    , AppState& state)
{
    m_world = world;
    m_window = window;
    m_phys = physiker;
    m_state = state;

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
    if      (front == "time"    || front == "t") { time   ::parse(command);}
    else if (front == "balls"   || front == "b") { balls  ::parse(command);}
    else if (front == "view"    || front == "v") { view   ::parse(command);}
    else if (front == "bounds"  || front =="bo") { bounds ::parse(command);}
    else if (front == "physics" || front == "p") { physics::parse(command);}
    else if (front == "load"    || front == "l") { load   ::parse(command);}
    else if (front == "quit"    || front == "q") { STATE = AppState::quit; }
    else { Debug::err("Invalid command"); }
    }
    catch (CommandException ex)
    {
        Debug::err("Something is wrong with your command :(");
    }
}

double InputHandler::makeDouble(string str, double min, double max)
{
    double r{};
    try   { r = std::stod(str); }
    catch (std::invalid_argument const&) 
        { throw CommandException::NumberExpected; }
    
    if (min <= r && r <= max) { return r; }

    throw CommandException::WrongParameter;
}
int InputHandler::makeInt(string str, int min, int max)
{
    int r{};
    try   { r = std::stoi(str); }
    catch (std::invalid_argument const&) 
        { throw CommandException::NumberExpected; }
    
    if (min <= r && r <= max) { return r; }

    throw CommandException::WrongParameter;
}
Time InputHandler::makeTime(string timeString, bool allowNegatives)
{
    int minint{0};
    double mindouble{0};
    if (allowNegatives)
    {
        minint = std::numeric_limits<int>::lowest();
        mindouble = std::numeric_limits<double>::lowest();
    }
    Time newTime{};

    if(unsuffix(timeString, "ns"))
    {
        newTime = Time::makeNS(makeInt(timeString, minint));
    }
    else if (unsuffix(timeString, "ms"))
    {
        newTime = Time::makeMS(makeInt(timeString, minint));
    }
    else
    {
        unsuffix(timeString, "s");
        newTime = Time::makeS(makeDouble(timeString, mindouble));
    }

    return newTime;
}
Eigen::Vector2d InputHandler::makeVector(std::string vectorString)
{
    if (unsuffix(vectorString, "randomize") || unsuffix(vectorString, "r"))
    {
        Eigen::Vector2d r {makeDouble(vectorString), 0};

        static std::uniform_real_distribution<double> unif{0, 2 * PI};
        static std::default_random_engine re;

        Eigen::Rotation2D rot {unif(re)};

        return rot * r;
    }

    queue<string> coords{splitString(vectorString, ';')};
    if (coords.size() != 2) { throw CommandException::WrongParameter; }

    double x{makeDouble(dequeue(coords))};
    double y{makeDouble(dequeue(coords))};

    return {x, y};
}
SDL_Color InputHandler::makeColor(std::string colorString)
{
    queue<string> values{splitString(colorString, ';')};
    if (3 > values.size() || values.size() > 4) 
        { throw CommandException::WrongParameter; }

    int r{makeInt(dequeue(values), 0, 255)};
    int g{makeInt(dequeue(values), 0, 255)};
    int b{makeInt(dequeue(values), 0, 255)};

    int a{255};
    if (!values.empty()) { a = makeInt(dequeue(values), 0, 255); }

    return {static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b)
        , static_cast<Uint8>(a)};
}

void InputHandler::outTime(Time time, queue<string>& command)
{
    string unit{};
    if(!command.empty())
    {
        unit = dequeue(command);
    }

    if (unit == "ns")
    {
        Debug::out(std::to_string(time.getNS()) + " nanoseconds");
    }
    else if (unit == "ms")
    {
        Debug::out(std::to_string(time.getMS()) + " milliseconds");
    }
    else
    {
        Debug::out(std::to_string(time.getS()) + " seconds");
    }
}

void InputHandler::time::parse(queue<string>& command)
{
    string front{dequeue(command)};

    if      (front == "set"   || front == "s") { set         (command); }
    else if (front == "move"  || front == "m") { move        (command); }
    else if (front == "get"   || front == "g") { get         (command); }
    else if (front == "scale" || front =="sc") { scale::parse(command); }
    else { throw CommandException::WrongArgument; }
}

void InputHandler::time::set(queue<string>& command)
{
    WINDOW.setTime(makeTime(dequeue(command)));
}
void InputHandler::time::move(queue<string>& command)
{
    string t{command.front()};

    WINDOW.setTime(WINDOW.getTime() + makeTime(t, true));
}
void InputHandler::time::get(queue<string>& command)
{
    outTime(WINDOW.getTime(), command);
}

void InputHandler::time::scale::parse(queue<string>& command)
{
    string front{dequeue(command)};

    if      (front == "set" || front == "s") { set(command); }
    else if (front == "get" || front == "g") { get();        }
    else { throw CommandException::WrongArgument; }
}
void InputHandler::time::scale::set(queue<string>& command)
{
    WINDOW.setTimescale(makeDouble(dequeue(command)));
}
void InputHandler::time::scale::get()
{
    Debug::out(std::to_string(WINDOW.getTimescale()));
}

void InputHandler::balls::parse(queue<string>& command)
{
    // doing this will hopefully prevent horrible multithreading bugs
    double oldTimescale{WINDOW.getTimescale()};
    WINDOW.setTimescale(0);
    Time oldRunahead{PHYS.getRunaheadTime()};
    PHYS.setRunaheadTime({});
    try {
    string front{dequeue(command)};

    if      (front == "get"    || front == "g") { get       (command); }
    else if (PHYS.isLoggingKineticEnergy())
    {
        Debug::err("Cannot use commands that alter world state while logging"
            " kinetic energy.");
    }
    else if (front == "new"    || front == "n") { newball   (command); }
    else if (front == "delete" || front == "d") { deleteball(command); }
    else if (front == "clear"  || front == "c") { clear     ();        }
    else { throw CommandException::WrongArgument; }
    }
    catch (...)
    {
        WINDOW.setTimescale(oldTimescale);
        PHYS.setRunaheadTime(oldRunahead);
        throw;
    }

    WINDOW.setTimescale(oldTimescale);
    PHYS.setRunaheadTime(oldRunahead);
}
void InputHandler::balls::get(queue<string>& command)
{
    string tag{};
    if (!command.empty())
    {
        string front{dequeue(command)};

        if(unprefix(front, "tag=") || unprefix(front, "t=")) { tag = front; }
    }

    for (const auto& b : WORLD.getBalls())
    {
        if (!tag.empty() && !b.hasTag(tag)) { continue; }

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
            + "position: " + addSpacing(positionX + ";" + positionY, 24) 
            + "velocity: " + addSpacing(velocityX + ";" + velocityY, 24) 
            + "color: " + addSpacing(cR + ";" + cG + ";" + cB, 24) 
            + "tags: " + b.getTagsAsString() + "\n");
    }
}
void InputHandler::balls::newball(queue<string>& command)
{
    double radius{1};
    Eigen::Vector2d position{0, 0};
    double mass{1};
    Eigen::Vector2d velocity{0, 0};
    SDL_Color color{255, 255, 255, 255};
    Time time{WINDOW.getTime()};
    std::deque<string> tags{};
    while (!command.empty())
    {
        string param{dequeue(command)};

        if      (unprefix(param, "radius=")   || unprefix(param, "r=")) 
            { radius   = makeDouble(param, 0);}
        else if (unprefix(param, "mass=")     || unprefix(param, "m="))
            { mass     = makeDouble(param, 0);}
        else if (unprefix(param, "position=") || unprefix(param, "p="))
            { position = makeVector(param);   }
        else if (unprefix(param, "velocity=") || unprefix(param, "v="))
            { velocity = makeVector(param);   }
        else if (unprefix(param, "color=")    || unprefix(param, "c="))
            { color    = makeColor (param);   }
        else if (unprefix(param, "tags=")     || unprefix(param, "t="))
            { tags = Ball::getTagsFromString(param); }
    }

    try
    {
    auto& b = WORLD.newBall(radius, position, mass, velocity, color, time);
    b.tags = tags;

    WINDOW.setTime({});
    PHYS.purgeKeyframes(time);
    }
    catch (WorldException ex)
    {
        Debug::err("You are trying to create a ball in an invalid position,"
            " such as inside another ball or outside the world bounds.");
    }
}
void InputHandler::balls::deleteball(queue<string>& command)
{
    string front{dequeue(command)};
    Debug::out(front);

    if (unprefix(front, "id="))
    {
        queue<string> IDs{splitString(front, ';')};
        
        while (!IDs.empty())
        {
            int ID{makeInt(dequeue(IDs))};

            auto& ballList{WORLD.getBallsModifiable()};
            
            try
            {
            ballList.erase(std::find(ballList.begin(), ballList.end()
                , WORLD.getBallByID(ID)));
            WINDOW.setTime({});
            PHYS.purgeKeyframes(WINDOW.getTime());
            }
            catch (WorldException ex)
            {
            Debug::err("Ball with ID " + std::to_string(ID) + " not found");
            }
        }
    }
    else { throw CommandException::WrongArgument; }
}
void InputHandler::balls::clear()
{
    WORLD.getBallsModifiable().clear();
}

void InputHandler::view::parse(queue<string>& command)
{
    string front{dequeue(command)};

    if      (front == "zoom"     || front == "z") { zoom::parse    (command); }
    else if (front == "position" || front == "p") { position::parse(command); }
    else { throw CommandException::WrongArgument; }
}

void InputHandler::view::zoom::parse(queue<string>& command)
{
    string front{dequeue(command)};

    if      (front == "set" || front == "s") { set(command); }
    else if (front == "get" || front == "g") { get();        }
    else { throw CommandException::WrongArgument; }
}
void InputHandler::view::zoom::set(queue<string>& command)
{
    WINDOW.setZoom(makeDouble(dequeue(command), 0.01));
}
void InputHandler::view::zoom::get()
{
    Debug::out(std::to_string(WINDOW.getZoom()));
}

void InputHandler::view::position::parse(queue<string>& command)
{
    string front{dequeue(command)};

    if      (front == "set"  || front == "s") { set (command); }
    else if (front == "move" || front == "m") { move(command); }
    else if (front == "get"  || front == "g") { get ();        }
    else { throw CommandException::WrongArgument; }
}
void InputHandler::view::position::set(queue<string>& command)
{
    WINDOW.setViewOffset(makeVector(dequeue(command)));
}
void InputHandler::view::position::move(queue<string>& command)
{
    WINDOW.setViewOffset(WINDOW.getViewOffset() + makeVector(dequeue(command)));
}
void InputHandler::view::position::get()
{
    Eigen::Vector2d v{WINDOW.getViewOffset()};
    Debug::out(std::to_string(v.x()) + ";" + std::to_string(v.y()));
}

void InputHandler::bounds::parse(queue<string>& command)
{
    string front{dequeue(command)};

    if      (front == "get" || front == "g") { get ();        }
    else if (PHYS.isLoggingKineticEnergy())
    {
        Debug::err("Cannot use commands that alter world state while logging"
            " kinetic energy.");
    }
    else if (front == "set" || front == "s") { set (command); }
    else { throw CommandException::WrongArgument; }
}
void InputHandler::bounds::set(queue<string>& command)
{
    if (command.front() == "clear" || command.front() == "c") 
    {
        WORLD.clearWorldBounds();
        return;
    }
    
    Rect newRect{WORLD.getWorldBounds().value_or(Rect{})};

    while (!command.empty())
    {
        string param{dequeue(command)};

        if (unprefix(param, "origin=") || unprefix(param, "o="))
        {
            auto temp = makeVector(param);
            newRect.x = temp.x();
            newRect.y = temp.y();
        }
        else if (unprefix(param, "size=") || unprefix(param, "s="))
        {
            auto temp = makeVector(param);
            newRect.w = temp.x();
            newRect.h = temp.y();
        }
    }

    try
    {
    WORLD.setWorldBounds(newRect, WINDOW.getTime());
    WINDOW.setTime({});
    PHYS.purgeKeyframes(WINDOW.getTime());
    }
    catch (WorldException ex)
    {
        Debug::err("The specified bounds would cause one or more balls to be"
            " out of bounds. This is not allowed.");
    }
}
void InputHandler::bounds::get()
{
    const auto& boundsopt{WORLD.getWorldBounds()};
    if (!boundsopt)
    {
        Debug::out("none");
        return;
    }
    const auto& bounds{boundsopt.value()};

    Debug::out("origin: " + std::to_string(bounds.x) + ";" 
        + std::to_string(bounds.x) + " size: " + std::to_string(bounds.w) + ";"
        + std::to_string(bounds.h));
}

void InputHandler::physics::parse(queue<string>& command)
{
    string front{dequeue(command)};

    if      (front == "runahead"     || front == "r") { runahead::parse  (command); }
    else if (front == "time"         || front == "t") { time             (command); }
    else if (front == "step"         || front == "s") { step::parse      (command); }
    else if (front == "iterations"   || front == "i") { iterations::parse(command); }
    else if (front == "kineticenergy"|| front == "k") { kineticEnergy    (command); }
    else if (front == "logkineticenergy"|| front == "l") 
        { logkineticenergy::parse(command); }
    else { throw CommandException::WrongArgument; }
}
void InputHandler::physics::time(queue<string>& command)
{
    outTime(PHYS.getSimulationTime(), command);
}

void InputHandler::physics::kineticEnergy(queue<string>& command)
{
    string tag{""};
    if (!command.empty())
    {
        string front{dequeue(command)};
        if (unprefix(front, "tag=") || unprefix(front, "t=")) { tag = front; }
        else { throw CommandException::WrongArgument; }
    }

    Debug::out(std::to_string(PHYS.getKineticEnergy(WINDOW.getTime(), tag)));
}

void InputHandler::physics::runahead::parse(queue<string>& command)
{
    string front{dequeue(command)};

    if      (front == "get" || front == "g") { get(command); }
    else if (front == "set" || front == "s") { set(command); }
    else { throw CommandException::WrongArgument; }
}
void InputHandler::physics::runahead::get(queue<string>& command)
{
    outTime(PHYS.getRunaheadTime(), command);
}
void InputHandler::physics::runahead::set(queue<string>& command)
{
    PHYS.setRunaheadTime(makeTime(dequeue(command)));
}

void InputHandler::physics::step::parse(queue<string>& command)
{
    string front{dequeue(command)};

    if      (front == "get" || front == "g") { get(command); }
    else if (PHYS.isLoggingKineticEnergy())
    {
        Debug::err("Cannot use commands that alter world state while logging"
            " kinetic energy.");
    }
    else if (front == "set" || front == "s") { set(command); }
    else { throw CommandException::WrongArgument; }
}
void InputHandler::physics::step::get(queue<string>& command)
{
    outTime(PHYS.getTimestep(), command);
}
void InputHandler::physics::step::set(queue<string>& command)
{
    PHYS.setTimestep(makeTime(dequeue(command)));
}

void InputHandler::physics::iterations::parse(queue<string>& command)
{
    string front{dequeue(command)};

    if      (front == "get" || front == "g") { get(); }
    else if (PHYS.isLoggingKineticEnergy())
    {
        Debug::err("Cannot use commands that alter world state while logging"
            " kinetic energy.");
    }
    else if (front == "set" || front == "s") { set(command); }
    else { throw CommandException::WrongArgument; }
}
void InputHandler::physics::iterations::get()
{
    Debug::out(std::to_string(PHYS.getMaxCollisionIterations()));
}
void InputHandler::physics::iterations::set(queue<string>& command)
{
    PHYS.setMaxCollisionIterations(makeInt(dequeue(command), 0));
}

void InputHandler::physics::logkineticenergy::parse(queue<string>& command)
{
    string front{dequeue(command)};

    if      (front == "begin" || front == "b") { logkineticenergy::begin(command); }
    else if (front == "end"   || front == "e") { logkineticenergy::end();   }
    else { throw CommandException::WrongArgument; }
}
void InputHandler::physics::logkineticenergy::begin(queue<string>& command)
{
    if (PHYS.isLoggingKineticEnergy())
    {
        Debug::err("Already logging kinetic energy.");
        return;
    }

    string filename{};
    Time interval{};
    std::deque<string> tags;

    while (!command.empty())
    {
        string param{dequeue(command)};

        if      (unprefix(param, "filename=") || unprefix(param, "f="))
            { filename = param; }
        else if (unprefix(param, "interval=") || unprefix(param, "i="))
            { interval = makeTime(param); }
        else if (unprefix(param, "tags=") || unprefix(param, "t="))
            { tags = Ball::getTagsFromString(param); }
    }

    if (filename.empty() || interval == Time{})
    {
        Debug::err("Filename or interval not provided; cannot begin logging.");
        return;
    }

    PHYS.beginLoggingKineticEnergy(filename, interval, tags);
}
void InputHandler::physics::logkineticenergy::end()
{
    if (!PHYS.isLoggingKineticEnergy())
    {
        Debug::err("Not logging kinetic energy.");
        return;
    }

    PHYS.stopLoggingKineticEnergy();
}

string InputHandler::load::fileBeingLoaded{};

void InputHandler::load::parse(queue<string>& command)
{
    string front{dequeue(command)};

    if (!unprefix(front, "file=") && !unprefix(front, "f="))
    {
        throw CommandException::WrongArgument;
    }

    std::ifstream file{front};

    if (!file)
    {
        Debug::err("File " + front + " could not be found.");
        return;
    }

    // makes sure the file is not loaded from within itself
    if (front == fileBeingLoaded)
    {
        Debug::err("Recursively loading the same file is not allowed.");
        return;
    }
    fileBeingLoaded = front;

    string input{};
    while (std::getline(file, input))
    {
        if (unprefix(input, "//") || input.empty())
        {
            continue;
        }
        Debug::out("Loaded command: " + input);
        parseInput(input);
    }

    fileBeingLoaded.clear();
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