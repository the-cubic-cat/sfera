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

#define million 1000000
#define billion 1000000000

class Time
{
public:
    Time(): m_timeNS{0} {}

    Time(const Time& t) = default;

    // set nanoseconds
    void setNS(int64_t nanoseconds) { m_timeNS = nanoseconds; }
    // get nanoseconds
    int64_t getNS() const { return m_timeNS; }

    // set milliseconds
    void setMS(int64_t milliseconds) { m_timeNS = milliseconds * million; }
    // get milliseconds
    int64_t getMS() const { return m_timeNS / million; }
    
    // set seconds with double (imprecise)
    void setS(double seconds) { setMS(static_cast<int64_t>(seconds * million)); }
    // get seconds double (imprecise)
    double getS() const { return static_cast<double>(m_timeNS) / billion; }

    Time getHalf() { return makeNS(m_timeNS / 2); }

    static Time makeNS(int64_t nanoseconds)
    {
        Time t{};
        t.setNS(nanoseconds);
        return t;
    }
    static Time makeMS(int64_t milliseconds)
    {
        Time t{};
        t.setMS(milliseconds);
        return t;
    }
    static Time makeS(double seconds)
    {
        Time t{};
        t.setS(seconds);
        return t;
    }

    Time& operator+=(const Time& t);
    Time& operator-=(const Time& t);

private:
    int64_t m_timeNS; // internally used integer time in nanoseconds
};
Time operator+ (const Time& a, const Time&   b);

Time operator- (const Time& a, const Time&   b);

bool operator< (const Time& a, const Time& b);
bool operator> (const Time& a, const Time& b);
bool operator==(const Time& a, const Time& b);
bool operator>=(const Time& a, const Time& b);
bool operator<=(const Time& a, const Time& b);

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