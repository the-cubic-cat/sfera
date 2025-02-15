#include "window.hpp"

Window::Window(AppState& state, const World& world)
    : m_rendererSDL{}
    , m_windowSDL{}
    , m_surfaceSDL{}
    , m_UIfontSDL{}
    , m_displayScale{100}
    , m_displayPositionOffset{540, 540}
    , m_state{state}
    , m_world{world}
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
        + m_displayPositionOffset;
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
        + m_displayPositionOffset);

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
        redraw();
    }
}

void Window::redraw()
{
    SDL_SetRenderDrawColor(m_rendererSDL, 0, 0, 0, 255);
    SDL_RenderClear(m_rendererSDL);
    //TODO: rework time to be non-shit
    Time time{};
    time.setMS(static_cast<int64_t>(SDL_GetTicks64()));
    for (const auto& b : m_world.getBalls())
    {
        drawCircle(b.getColor(), b.getPositionAtTime(time)
            , b.getRadius(), true);
    }
    
    drawRect({0, 0, 255, 255}
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