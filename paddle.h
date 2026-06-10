#ifndef PADDLE_H
#define PADDLE_H

#include <ncurses.h>
#include "vec2.h"
#include "arena.h"

class paddle {
    public:
    paddle(int size, vec2 position) : size(size), position(position) {};

    void movePaddle(int amount, ArenaBorder arena){
        if(((amount == -1) && (position.gety() > 1)) || ((amount == 1) && ((position.gety() + size) < (arena.getH()-1)))){
            position.sety(position.gety() + amount);
        };
    };

    void renderPaddle(){
        mvvline(position.gety(), position.getx(), '|', size);
    }

    int getSize(){
        return size;
    }

    int getPosY(){
        return position.gety();
    }

    int getPosX(){
        return position.getx();
    }



    private:
    int size;
    vec2 position;
};

#endif