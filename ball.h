#ifndef BALL_H
#define BALL_H

#include <iostream>
#include "vec2.h"

class ball {
    public:
    ball(double radius, vec2 position, vec2 velocity) : radius(radius), position(position), velocity(velocity){};

    

    private:
    double radius;
    vec2 position;
    vec2 velocity;
};

#endif