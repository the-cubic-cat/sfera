#include "window.hpp"

Window::Window(AppState& state, const std::vector<Ball>& balls)
    : m_balls{balls}
    , m_state{state}
    , m_rendererSDL{}
    , m_windowSDL{}
    , m_surfaceSDL{}
    , m_UIfontSDL{}
    , m_displayScale{100}
    , m_displayPositionOffset{500, 500}
{
    Debug::log("Creating window...");
    // basic setup
    if (m_windowSDL == nullptr)
    {
        SDL_Init(SDL_INIT_VIDEO);
    }
    SDL_CreateWindowAndRenderer(m_defaultWidth, m_defaultHeight,
    SDL_WINDOW_ALLOW_HIGHDPI, &m_windowSDL, &m_rendererSDL);

    SDL_SetWindowTitle(m_windowSDL, "Physics Simulation");
    SDL_SetWindowMinimumSize(m_windowSDL, m_minWidth, m_minHeight);
    SDL_SetRenderDrawColor(m_rendererSDL, 0, 0, 0, 0);
    SDL_RenderClear(m_rendererSDL);
    m_surfaceSDL = SDL_GetWindowSurface(m_windowSDL);
    // fonts
    if (TTF_Init() < 0)
    {
        Debug::log("Error initializing SDL_ttf!");
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_SetWindowResizable(m_windowSDL, SDL_TRUE);

    m_UIfontSDL = TTF_OpenFont("data/LiberationSans-Regular.ttf", 38);
    if (!m_UIfontSDL)
    {
        Debug::log("Failed to load font!");
    }

    Debug::log("Window created.");

    drawLoop();
}

Window::~Window()
{
    Debug::log("Destroying window...");
    // close fonts
    TTF_CloseFont(m_UIfontSDL);
    TTF_Quit();
    // clear surface
    SDL_FreeSurface(m_surfaceSDL);
    m_surfaceSDL = nullptr;
    // Destroy window
    SDL_DestroyWindow(m_windowSDL);
    m_UIfontSDL = nullptr;

    // Quit SDL subsystems
    SDL_Quit();
    Debug::log("Window destroyed.");
}

void Window::drawCircle(Color color, Eigen::Vector2d position, double radius
    , bool filled)
{
    SDL_Color sdlC{color.r, color.g, color.b, 255};
    Vector2d screenPosition = position * m_displayScale 
        + m_displayPositionOffset;
    int screenRadius = (int)(radius * m_displayScale);

    if (filled)
    {
        screenDrawFilledCircle(sdlC, (int)screenPosition.x()
            , (int)screenPosition.y(), screenRadius);
        return;
    }
    screenDrawCircle(sdlC, (int)screenPosition.x(), (int)screenPosition.y()
        , screenRadius);
}

void Window::drawLoop()
{
    while (m_state == AppState::simulation)
    {
        redraw();
    }
}

void Window::redraw()
{
    SDL_SetRenderDrawColor(m_rendererSDL, 0, 0, 0, 255);
    SDL_RenderClear(m_rendererSDL);
    //TODO: rework time to be non-shit
    double time{(double)SDL_GetTicks64() / 1000};
    for (Ball b : m_balls)
    {
        drawCircle({255, 255, 255}, b.getPositionAtTime(time), b.getRadius()
        , true);
    }
    SDL_RenderPresent(m_rendererSDL);
    // Utils::Out("redrew");
}

void Window::screenDrawRectangle(SDL_Color color, SDL_Rect rect)
{
    SDL_SetRenderDrawColor(m_rendererSDL, color.r, color.g, color.b, 255);
    SDL_RenderFillRect(m_rendererSDL, &rect);
}
// stolen: https://gist.github.com/Gumichan01/332c26f6197a432db91cc4327fcabb1c
int Window::screenDrawCircle(SDL_Color color, int x, int y, int radius)
{
    SDL_SetRenderDrawColor(m_rendererSDL, color.r, color.g, color.b, color.a);

    int offsetx, offsety, d;
    int status;

    offsetx = 0;
    offsety = radius;
    d = radius - 1;
    status = 0;

    while (offsety >= offsetx)
    {
        status += SDL_RenderDrawPoint(m_rendererSDL, x + offsetx, y + offsety);
        status += SDL_RenderDrawPoint(m_rendererSDL, x + offsety, y + offsetx);
        status += SDL_RenderDrawPoint(m_rendererSDL, x - offsetx, y + offsety);
        status += SDL_RenderDrawPoint(m_rendererSDL, x - offsety, y + offsetx);
        status += SDL_RenderDrawPoint(m_rendererSDL, x + offsetx, y - offsety);
        status += SDL_RenderDrawPoint(m_rendererSDL, x + offsety, y - offsetx);
        status += SDL_RenderDrawPoint(m_rendererSDL, x - offsetx, y - offsety);
        status += SDL_RenderDrawPoint(m_rendererSDL, x - offsety, y - offsetx);

        if (status < 0)
        {
            status = -1;
            break;
        }

        if (d >= 2 * offsetx)
        {
            d -= 2 * offsetx + 1;
            offsetx += 1;
        }
        else if (d < 2 * (radius - offsety))
        {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else
        {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

    return status;
}

int Window::screenDrawFilledCircle(SDL_Color color, int x, int y, int radius)
{
    SDL_SetRenderDrawColor(m_rendererSDL, color.r, color.g, color.b, color.a);

    int offsetx, offsety, d;
    int status;

    offsetx = 0;
    offsety = radius;
    d = radius - 1;
    status = 0;

    while (offsety >= offsetx)
    {

        status += SDL_RenderDrawLine(m_rendererSDL, x - offsety, y + offsetx,
                                     x + offsety, y + offsetx);
        status += SDL_RenderDrawLine(m_rendererSDL, x - offsetx, y + offsety,
                                     x + offsetx, y + offsety);
        status += SDL_RenderDrawLine(m_rendererSDL, x - offsetx, y - offsety,
                                     x + offsetx, y - offsety);
        status += SDL_RenderDrawLine(m_rendererSDL, x - offsety, y - offsetx,
                                     x + offsety, y - offsetx);

        if (status < 0)
        {
            status = -1;
            break;
        }

        if (d >= 2 * offsetx)
        {
            d -= 2 * offsetx + 1;
            offsetx += 1;
        }
        else if (d < 2 * (radius - offsety))
        {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else
        {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

    return status;
}