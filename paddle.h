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
        mvvline(position.gety(), position.getx(), '|', size);
    }

    private:
    int size;
    vec2 position;
};

#endif