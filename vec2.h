#ifndef VEC2_H
#define VEC2_H

#include <random>
#include <cmath>

inline double random_double();

class vec2{
    public:
    vec2(double x, double y) : x(x), y(y) {}

    double getx(){
        return x;
    }
    double gety(){
        return y;
    }

    void setx(double pos){
        x = pos;
    }
    void sety(double pos){
        y = pos;
    }
    static vec2 randomUV(){ // Generate a Random Unit Vector
        vec2 TempVec(random_double(), random_double());
        TempVec /= sqrt(TempVec.x*TempVec.x + TempVec.y*TempVec.y);
        return TempVec;
    }

    vec2& operator/=(double d){
        x /= d;
        y /= d;
        return *this;
    }

    private:
    double x;
    double y;
};

inline double random_double(){
    static std::uniform_real_distribution<double> distribution(-1.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}


#endif