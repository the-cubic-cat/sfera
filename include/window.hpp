#pragma once

#include "inputer.hpp"

// stolen: https://gamedev.stackexchange.com/questions/110825/how-to-calculate-delta-time-with-sdl
struct Clock
{
    uint64_t last_tick_time{0};
    uint64_t delta{0};

    void tick()
    {
        uint64_t tick_time = SDL_GetTicks64();
        delta = tick_time - last_tick_time;
        last_tick_time = tick_time;
    }
};

// class handling the window and drawing in it
class Window
{
public:
    // create a new window
    Window(const AppState& state, const World& world, Time& currentTime);
    // destroy the window
    virtual ~Window();

    void drawCircle(SDL_Color color, Eigen::Vector2d position, double radius
        , bool filled = true);
    
    void drawRect(SDL_Color color, Rect rect, bool filled = true);

    // begins executing draw loop. it will run while m_state == simulation.
    void loop();

    void setTime(Time newTime) { m_time = newTime; }
    Time getTime() const { return m_time; }

    void setTimescale(double newScale) { m_timescale = newScale; }
    double getTimescale() const { return m_timescale; }

    void setZoom(double zoom) { m_displayScale = zoom * 100; }
    double getZoom() const { return m_displayScale / 100; }

    void setViewOffset(Eigen::Vector2d offset)
    {
        int w{};
        int h{};
        SDL_GetWindowSize(m_windowSDL, &w, &h);
        m_displayPositionOffset = offset * m_displayScale 
            + Eigen::Vector2d{static_cast<double>(w) / 2
            , static_cast<double>(h) / 2};
    }
    Eigen::Vector2d getViewOffset() const
    {
        int w{};
        int h{};
        SDL_GetWindowSize(m_windowSDL, &w, &h);
        
        return (m_displayPositionOffset - Eigen::Vector2d{static_cast<double>(w) 
            / 2, static_cast<double>(h) / 2}) / m_displayScale ;
    }

    // it doesn't make sense for windows to be copied or moved
    Window(const Window& window) = delete;
    Window& operator=(const Window& window) = delete;

    Window(Window&& window) = delete;
    Window& operator=(Window&& window) = delete;

private:

    // draw all that needs to be drawn
    void redraw();

    void screenDrawRect(SDL_Color color, SDL_Rect rect);

    void screenDrawFilledRect(SDL_Color color, SDL_Rect rect);

    int screenDrawCircle(SDL_Color color, int x, int y, int radius);

    int screenDrawFilledCircle(SDL_Color color, int x, int y, int radius);

    static constexpr int m_defaultWidth{1080};
    static constexpr int m_defaultHeight{1080};
    static constexpr int m_minWidth{300};
    static constexpr int m_minHeight{300};

    SDL_Renderer *m_rendererSDL;
    SDL_Window *m_windowSDL;
    SDL_Surface *m_surfaceSDL;

    TTF_Font *m_UIfontSDL;

    double m_displayScale;
    Eigen::Vector2d m_displayPositionOffset;
    double m_timescale;
    Time m_time;
    Clock m_clock;

    const AppState& m_state;
    const World& m_world;
    Time& m_currentTime;
};