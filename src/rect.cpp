#include "rect.hpp"

Rect operator*(const Rect& r, const double& d)
{
    return {r.x * d, r.y * d, r.w * d, r.h * d};
}

Rect operator+(const Rect& r, const Eigen::Vector2d& v)
{
    return {r.x + v.x(), r.y + v.y(), r.w, r.h};
}

Rect::operator SDL_Rect() const
{
    return {static_cast<int>(x), static_cast<int>(y)
        , static_cast<int>(w), static_cast<int>(h)};
}