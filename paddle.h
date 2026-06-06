#ifndef PADDLE_H
#define PADDLE_H

#include <ncurses.h>
#include "vec2.h"

class paddle {
    public:
    paddle(int size, vec2 position) : size(size), position(position) {};

    void movePaddle(int amount){
        position.sety(position.gety() + amount);
    };

    void renderPaddle(){
        mvvline(position.getx(), position.gety(), '|', size);
    }

    private:
    vec2 position;
    int size;
};

#endif