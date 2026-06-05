#ifndef ARENA_H
#define ARENA_H

class border{
    public:
    border(double height, double width) : height(height), width(width) {};

    double getH(){
        return height;
    }

    double getW(){
        return width;
    }

    private:
    double height;
    double width;
};

#endif