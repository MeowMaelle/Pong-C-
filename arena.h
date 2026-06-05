#ifndef ARENA_H
#define ARENA_H

#include <iostream>
#include <ncurses.h>

class ArenaBorder{
    public:
    ArenaBorder(int height, int width) : height(height), width(width) {};

    int getH(){
        return height;
    }

    int getW(){
        return width;
    }

    void renderArena(){
        mvhline(0, 0, '-', width);
        mvhline(height-1, 0, '-', width);
        mvvline(1, 0, '|', height-2);
        mvvline(1, width-1, '|', height-2);
    }

    private:
    int height;
    int width;
};

#endif