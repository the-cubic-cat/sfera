#pragma once

#include "world.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Window
{
public:
    // draw all that needs to be drawn
    void redraw();

    // create a new window
    Window();
    // destroy the window and kill the thread
    ~Window();

    // it doesn't make sense for windows to be copied or moved
    Window(const Window& window) = delete;
    Window& operator=(const Window& window) = delete;

    Window(Window&& window) = delete;
    Window& operator=(Window&& window) = delete;

private:
    void drawRectangle(SDL_Color color, SDL_Rect rect);

    int drawCircle(SDL_Color color, int x, int y, int radius);

    int drawFilledCircle(SDL_Color color, int x, int y, int radius);

    static constexpr int m_defaultWidth{1080};
    static constexpr int m_defaultHeight{1080};
    static constexpr int m_minWidth{300};
    static constexpr int m_minHeight{300};

    SDL_Renderer *m_rendererSDL;
    SDL_Window *m_windowSDL;
    SDL_Surface *m_surfaceSDL;

    TTF_Font *m_UIfontSDL;

    float m_displayScale;
    Eigen::Vector2d m_displayPositionOffset;
};