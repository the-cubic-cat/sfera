#pragma once

#include "window.hpp"
#include "physics.hpp"
#include <random>

class InputHandler
{
public:
    static void init(World& world, Window& window, Physiker& physiker
        , AppState& state);
    static void parseInput(std::string inputCommand);
    static void checkWaiting();
private:
    
    #define COMMAND std::queue<std::string>

    static void runCommand(COMMAND& command);

    static bool m_initalized;
    static std::string m_helpText;

    // unfathomably ugly, but i fear raw pointers like the plague
    static std::optional<std::reference_wrapper<World>>    m_world;
    #define WORLD m_world.value().get()

    static std::optional<std::reference_wrapper<Window>>   m_window;
    #define WINDOW m_window.value().get()

    static std::optional<std::reference_wrapper<Physiker>> m_phys;
    #define PHYS m_phys.value().get()

    static std::optional<std::reference_wrapper<AppState>> m_state;
    #define STATE m_state.value().get()

    static double makeDouble(std::string str
        , double min = std::numeric_limits<double>::lowest()
        , double max = std::numeric_limits<double>::max());
    static int makeInt(std::string str
        , int min = std::numeric_limits<int>::lowest()
        , int max = std::numeric_limits<int>::max());
    
    static Time makeTime(std::string timeString, bool allowNegatives = false);
    static Eigen::Vector2d makeVector(std::string vectorString);
    static SDL_Color makeColor(std::string colorString);

    static void outTime(Time time, COMMAND& command);

    class load
    {
    public:
        static void parse(COMMAND& command);
    private:
        static std::string fileBeingLoaded; 
    };

    class time
    {
    public:
        static void parse(COMMAND& command);
    private:
        static void set(COMMAND& command);
        static void move(COMMAND& command);
        static void get(COMMAND& command);
        class scale
        {
        public:
            static void parse(COMMAND& command);
        private:
            static void set(COMMAND& command);
            static void get();
        };
    };
    
    class balls
    {
    public:
        static void parse(COMMAND& command);
    private:
        static void get(COMMAND& command);
        static void newball(COMMAND& command);
        static void deleteball(COMMAND& command);
        static void clear();
    };

    class view
    {
    public:
        static void parse(COMMAND& command);
    private:
        class zoom
        {
        public:
            static void parse(COMMAND& command);
        private:
            static void set(COMMAND& command);
            static void get();
        };

        class position
        {
        public:
            static void parse(COMMAND& command);
        private:
            static void set(COMMAND& command);
            static void move(COMMAND& command);
            static void get();
        };
    };

    class bounds
    {
    public:
        static void parse(COMMAND& command);
    private:
        static void set(COMMAND& command);
        static void get();
    };

    class physics
    {
    public:
        static void parse(COMMAND& command);
    private:
        static void time(COMMAND& command);
        static void kineticEnergy(COMMAND& command);
        class runahead
        {
        public:
            static void parse(COMMAND& command);
        private:
            static void get(COMMAND& command);
            static void set(COMMAND& command);
        };
        class step
        {
        public:
            static void parse(COMMAND& command);
        private:
            static void get(COMMAND& command);
            static void set(COMMAND& command);
        };
        class iterations
        {
        public:
            static void parse(COMMAND& command);
        private:
            static void get();
            static void set(COMMAND& command);
        };
        class logkineticenergy
        {
        public:
            static void parse(COMMAND& command);
        private:
            static void begin(COMMAND& command);
            static void end();
        };
    };

    class wait
    {
    public:
        static std::vector<std::pair<Time, COMMAND>> m_phys;
        static std::vector<std::pair<Time, COMMAND>> m_time;

        static void parse(COMMAND& command);
    };
};

std::string makeLowercase(std::string str);

enum class CommandException
{
    MissingArgument,
    WrongArgument,
    NumberExpected,
    WrongParameter
};

std::string addSpacing(std::string str, int desiredLength);

template <typename T>
T dequeue(std::queue<T>& queue)
{
    if (queue.empty()) { throw CommandException::MissingArgument; }

    T r{queue.front()};
    queue.pop();
    return r;
}