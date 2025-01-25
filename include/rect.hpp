#pragma once

#include "base.hpp"

struct Rect
{
    double x, y; // all in meters
    double w, h;

    explicit operator SDL_Rect() const;
};

Rect operator*(const Rect& r, const double& d);

Rect operator+(const Rect& r, const Eigen::Vector2d& v);