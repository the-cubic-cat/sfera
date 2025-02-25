#pragma once

#include "window.hpp"
#include "physics.hpp"

class InputHandler
{
public:
    static void init(World& world, Window& window, Physiker& physiker
        , AppState& state);
    static void parseInput(std::string inputCommand);
private:
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

    static void outTime(Time time, std::queue<std::string> command);

    class time
    {
    public:
        static void parse(std::queue<std::string> command);
    private:
        static void set(std::queue<std::string> command);
        static void move(std::queue<std::string> command);
        static void get(std::queue<std::string> command);
        class scale
        {
        public:
            static void parse(std::queue<std::string> command);
        private:
            static void set(std::queue<std::string> command);
            static void get();
        };
    };
    
    class balls
    {
    public:
        static void parse(std::queue<std::string> command);
    private:
        static void get(std::queue<std::string> command);
        static void newball(std::queue<std::string> command);
        static void deleteball(std::queue<std::string> command);
        static void clear();
    };

    class view
    {
    public:
        static void parse(std::queue<std::string> command);
    private:
        class zoom
        {
        public:
            static void parse(std::queue<std::string> command);
        private:
            static void set(std::queue<std::string> command);
            static void get();
        };

        class position
        {
        public:
            static void parse(std::queue<std::string> command);
        private:
            static void set(std::queue<std::string> command);
            static void move(std::queue<std::string> command);
            static void get();
        };
    };

    class bounds
    {
    public:
        static void parse(std::queue<std::string> command);
    private:
        static void set(std::queue<std::string> command);
        static void get();
    };

    class physics
    {
    public:
        static void parse(std::queue<std::string> command);
    private:
        static void time(std::queue<std::string> command);
        class runahead
        {
        public:
            static void parse(std::queue<std::string> command);
        private:
            static void get(std::queue<std::string> command);
            static void set(std::queue<std::string> command);
        };
        class step
        {
        public:
            static void parse(std::queue<std::string> command);
        private:
            static void get(std::queue<std::string> command);
            static void set(std::queue<std::string> command);
        };
        class iterations
        {
        public:
            static void parse(std::queue<std::string> command);
        private:
            static void get();
            static void set(std::queue<std::string> command);
        };
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