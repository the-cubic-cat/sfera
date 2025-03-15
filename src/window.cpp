#include "window.hpp"

using Eigen::Vector2d;

Window::Window(const AppState& state, const World& world, Time& currentTime)
    : m_rendererSDL{}
    , m_windowSDL{}
    , m_surfaceSDL{}
    , m_UIfontSDL{}
    , m_displayScale{100}
    , m_displayCenter{540, 540}
    , m_viewOffset{0, 0}
    , m_timescale{1}
    , m_time{}
    , m_clock{}
    , m_state{state}
    , m_world{world}
    , m_currentTime{currentTime}
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

void Window::drawCircle(SDL_Color color, Eigen::Vector2d position, double radius
    , bool filled)
{
    Vector2d screenPosition = position * m_displayScale 
        + m_displayCenter + m_viewOffset * m_displayScale;
    int screenRadius = static_cast<int>(radius * m_displayScale);

    if (filled)
    {
        screenDrawFilledCircle(color, (int)screenPosition.x()
            , (int)screenPosition.y(), screenRadius);
        /*screenDrawFilledRect({255, 0, 0, 255}, {(int)screenPosition.x()
            , (int)screenPosition.y(), 5, 5});*/
        return;
    }
    screenDrawCircle(color, (int)screenPosition.x(), (int)screenPosition.y()
        , screenRadius);
}

void Window::drawRect(SDL_Color color, Rect rect, bool filled)
{
    SDL_Rect screenRect = static_cast<SDL_Rect>(rect * m_displayScale 
        + m_displayCenter + m_viewOffset * m_displayScale);

    if (filled)
    {
        screenDrawFilledRect(color, screenRect);
        return;
    }
    screenDrawRect(color, screenRect);
}

void Window::loop()
{
    while (m_state == AppState::simulation)
    {
        // placeholder
        SDL_Event eventSDL;
        while (SDL_PollEvent(&eventSDL) != 0)
        {
            // makes resizing window slightly less of a pain
            switch (eventSDL.type)
            {
            case SDL_WINDOWEVENT:
            switch (eventSDL.window.event)
            {
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                recenterView();
                break;
            }
            }
        }
        m_currentTime = m_time;

        if (m_world.isBeingEdited) { continue; }

        redraw();
    }
}

void Window::redraw()
{
    SDL_SetRenderDrawColor(m_rendererSDL, 255, 255, 255, 255);
    SDL_RenderClear(m_rendererSDL);

    m_clock.tick();    
    if (m_time < m_world.endTime)
    {
        m_time += Time::makeS(static_cast<double>(m_clock.delta) / 1000 * m_timescale);
    }
    else { m_time = m_world.endTime; }
    //m_time = Time::makeMS(SDL_GetTicks64());

    //std::cout << m_time.getS() << "\n";

    try
    {
    for (const auto& b : m_world.getBalls())
    {
        if (m_world.isBeingEdited) { continue; }
        drawCircle(b.getColor(), b.getPositionAtTime(m_time)
            , b.getRadius(), true);
    }
    }
    catch (WorldException ex)
    {
        switch (ex)
        {
        case WorldException::TimeInaccessible:
            Debug::err("Time is inaccessible: " + std::to_string(m_time.getS())
                + ". Setting time to 0 and pausing.");
            m_timescale = 0;
            m_time = Time::makeNS(1);
            break;
        default:
            break;
        }
    }
    
    drawRect({0, 0, 0, 255}
        , m_world.getWorldBounds().value_or<Rect>({0, 0, 0, 0}), false);

    SDL_RenderPresent(m_rendererSDL);
    // Utils::Out("redrew");
}

void Window::screenDrawRect(SDL_Color color, SDL_Rect rect)
{
    SDL_SetRenderDrawColor(m_rendererSDL, color.r, color.g, color.b, 255);
    SDL_RenderDrawRect(m_rendererSDL, &rect);
}
void Window::screenDrawFilledRect(SDL_Color color, SDL_Rect rect)
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