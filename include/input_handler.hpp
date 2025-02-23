#pragma once

#include "window.hpp"
#include "physics.hpp"
#include <queue>

class InputHandler
{
public:
    static void init(World& world, Window& window, Physiker& physiker);
    static void parseInput(std::string inputCommand);
private:
    static bool m_initalized;
    static std::string helpText;

    // unfathomably ugly, but i fear raw pointers like the plague
    static std::optional<std::reference_wrapper<World>>    m_world;
    #define WORLD m_world.value().get()

    static std::optional<std::reference_wrapper<Window>>   m_window;
    #define WINDOW m_window.value().get()

    static std::optional<std::reference_wrapper<Physiker>> m_phys;
    #define PHYS m_phys.value().get()

    static std::queue<std::string> splitString(std::string str, char separator);
    static Time makeTime(std::string timeString);
    static Eigen::Vector2d makeVector(std::string vectorString);
    static SDL_Color makeColor(std::string colorString);

    class graphics
    {
    public:
        static void parse(std::queue<std::string> command);
    private:
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
    };
    
    class world
    {
    public:
        static void parse(std::queue<std::string> command);
    private:
        class balls
        {
        public:
            static void parse(std::queue<std::string> command);
        private:
            static void get();
            static void newball(std::queue<std::string> command);
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