#include "utils.hpp"
#include <iostream>

Time operator+(const Time& a, const Time& b) 
{ return Time::makeNS(a.getNS() + b.getNS()); }

Time operator-(const Time& a, const Time& b) 
{ return Time::makeNS(a.getNS() - b.getNS()); }

Time& Time::operator+=(const Time& t) 
{
    m_timeNS += t.m_timeNS;
    return *this;
}
Time& Time::operator-=(const Time& t) 
{
    m_timeNS -= t.m_timeNS;
    return *this;
}

bool operator< (const Time& a, const Time& b){ return a.getNS() < b.getNS(); }
bool operator> (const Time& a, const Time& b){ return a.getNS() > b.getNS(); }
bool operator==(const Time& a, const Time& b){ return a.getNS() ==b.getNS(); }
bool operator>=(const Time& a, const Time& b){ return a.getNS() >=b.getNS(); }
bool operator<=(const Time& a, const Time& b){ return a.getNS() <=b.getNS(); }

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

Eigen::Vector2d flipVector2d(Eigen::Vector2d v, Axis axis)
{
    switch (axis)
    {
    case Axis::Y:
        return {v.x(), -v.y()};
        break;

    case Axis::X:
        return {-v.x(), v.y()};
        break;
    }
    return v;
}

Eigen::Vector2d reflectVector2d(Eigen::Vector2d v, Eigen::Rotation2Dd angle)
{
    using Eigen::Vector2d;
    using Eigen::Rotation2Dd;

    Rotation2Dd negAngle{angle.inverse()};
    Vector2d v_{negAngle * v};

    Vector2d r_{flipVector2d(v_, Axis::Y)};

    Vector2d r{angle * r_};

    return r;
    //return angle * flipVector2d((negAngle * v), Axis::Y);
}

bool unsuffix(std::string& suffixedString, std::string suffix)
{
    std::size_t sufLen{suffix.size()};
    std::size_t strLen{suffixedString.size()};

    if (strLen <= sufLen || !suffixedString.ends_with(suffix)) { return false; }

    suffixedString = suffixedString.substr(0, strLen - sufLen);
    return true;
}

bool unprefix(std::string& prefixedString, std::string prefix)
{
    std::size_t preLen{prefix.size()};
    std::size_t strLen{prefixedString.size()};

    if (strLen <= preLen || prefixedString.rfind(prefix, 0) != 0) { return false; }

    prefixedString.erase(prefixedString.begin(), prefixedString.begin()
        + static_cast<int>(preLen));
    return true;
}

std::deque<std::string> splitString(const std::string& str, char separator)
{
    std::deque<std::string> r{};

    std::stringstream words(str);
    std::string tempWord;

    while (getline(words, tempWord, separator))
    {
        r.push_back(tempWord);
    }

    return r;
}