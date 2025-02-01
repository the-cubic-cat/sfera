#pragma once

#include "base.hpp"

struct Rect
{
    double x, y; // all in meters
    double w, h;

    explicit operator SDL_Rect() const;\

    // grow rect in all directions by value
    Rect growBy(double value) const;
    // check if vector is inside the bounds of the rect
    bool contains(Eigen::Vector2d vector) const;
};

Rect operator*(const Rect& r, const double& d);

Rect operator+(const Rect& r, const Eigen::Vector2d& v);

bool inRange(double p, double v1, double v2);