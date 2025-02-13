#include "utils.hpp"
#include <iostream>

Time operator+(const Time& a, const Time& b) { return a.getMS() + b.getMS(); }
Time operator+(const Time& a, const double& b)
    { return a.getMS() + std::lround(b * 1000.0); }

Time operator-(const Time& a, const Time& b) { return a.getMS() - b.getMS(); }
Time operator-(const Time& a, const double& b)
    { return a.getMS() - std::lround(b * 1000.0); }

Time operator< (const Time& a, const Time& b){ return a.getMS() < b.getMS(); }
Time operator> (const Time& a, const Time& b){ return a.getMS() > b.getMS(); }
Time operator==(const Time& a, const Time& b){ return a.getMS() ==b.getMS(); }
Time operator>=(const Time& a, const Time& b){ return a.getMS() >=b.getMS(); }
Time operator<=(const Time& a, const Time& b){ return a.getMS() <=b.getMS(); }

Rect::operator SDL_Rect() const
{
    return {static_cast<int>(x), static_cast<int>(y)
        , static_cast<int>(w), static_cast<int>(h)};
}

Rect Rect::growBy(double value) const
{
    double wChange = value * std::copysign(1, w);
    double hChange = value * std::copysign(1, h);

    return
        { x - wChange
        , y - hChange
        , w + 2 * wChange
        , h + 2 * hChange };
}

bool Rect::contains(Eigen::Vector2d vector) const
{
    if (inRange(vector.x(), x, x + w) && inRange(vector.y(), y, y + h))
    {
        return true;
    }
    return false;
}

double Rect::getTop()
{
    if (y < y + h)
    {
        return y;
    }
    return y + h;
}

double Rect::getBottom()
{
    if (y > y + h)
    {
        return y;
    }
    return y + h;
}

double Rect::getLeft()
{
    if (x < x + w)
    {
        return x;
    }
    return x + w;
}

double Rect::getRight()
{
    if (x > x + w)
    {
        return x;
    }
    return x + w;
}

Rect operator*(const Rect& r, const double& d)
{
    return {r.x * d, r.y * d, r.w * d, r.h * d};
}

Rect operator+(const Rect& r, const Eigen::Vector2d& v)
{
    return {r.x + v.x(), r.y + v.y(), r.w, r.h};
}

// stolen: https://stackoverflow.com/questions/54118163/most-efficient-way-to-determine-if-value-between-two-other-values-inclusive
bool inRange(double p, double v1, double v2) { return (p < v1) != (p < v2); }