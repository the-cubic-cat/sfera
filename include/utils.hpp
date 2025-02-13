#pragma once

#include "base.hpp"
#include <unordered_set>

enum class Direction
{
    none,
    up,
    down,
    left,
    right
};

class Time
{
public:
    Time(): m_timeMS{0} {}
    // INITIALIZED WITH SECONDS
    Time(double timeS) : m_timeMS{std::lround(timeS * 1000)} {}

    // directly set underlying milliseconds
    void setMS(int64_t milliseconds) { m_timeMS = milliseconds; }
    // get underlying milliseconds
    int64_t getMS() const { return m_timeMS; }
    //get seconds
    operator double() const { return static_cast<double>(m_timeMS * 1000); }
private:
    int64_t m_timeMS; // internally used integer time
};
Time operator+ (const Time& a, const Time&   b);
Time operator+ (const Time& a, const double& b);

Time operator- (const Time& a, const Time&   b);
Time operator- (const Time& a, const double& b);

Time operator< (const Time& a, const Time& b);
Time operator> (const Time& a, const Time& b);
Time operator==(const Time& a, const Time& b);
Time operator>=(const Time& a, const Time& b);
Time operator<=(const Time& a, const Time& b);

struct Rect
{
    double x, y; // all in meters
    double w, h;

    explicit operator SDL_Rect() const;

    // grow rect in all directions by value
    Rect growBy(double value) const;
    // check if vector is inside the bounds of the rect
    bool contains(Eigen::Vector2d vector) const;
    // get top bound of rect
    double getTop();
    // get bottom bound of rect
    double getBottom();
    // get left bound of rect
    double getLeft();
    // get right bound of rect
    double getRight();
};

Rect operator*(const Rect& r, const double& d);

Rect operator+(const Rect& r, const Eigen::Vector2d& v);



bool inRange(double p, double v1, double v2);

template <typename T>
std::vector<T> vectRemoveDuplicates(std::vector<T> v)
{
    // stolen: https://www.geeksforgeeks.org/remove-duplicates-from-vector-in-cpp/
    std::unordered_set<T> us;
    auto it = v.begin();

    while (it != v.end()) {

        // If elements exist in us then current
      	// copy is duplicate, so remove it
        if (us.find(*it) != us.end())
            it = v.erase(it);
      
      	// If element is not present in us,
      	// then it is unique so insesrt it in us
        else {
            us.insert(*it);
            it++;
        }
    }
    return v;
}