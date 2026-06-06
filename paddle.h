#ifndef PADDLE_H
#define PADDLE_H

#include <ncurses.h>

class paddle {
    public:
    paddle(int size, double velocity) : size(size), velocity(velocity) {};

    void movePaddle(int amount){
        
    };

    private:
    double velocity;
    int size;
};

#endif