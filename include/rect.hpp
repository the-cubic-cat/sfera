#pragma once

#include "base.hpp"

enum class Direction
{
    none,
    up,
    down,
    left,
    right
};

struct Rect
{
    double x, y; // all in meters
    double w, h;

    explicit operator SDL_Rect() const;\

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