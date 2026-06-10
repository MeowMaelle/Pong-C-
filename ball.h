#ifndef BALL_H
#define BALL_H

#include <iostream>
#include "vec2.h"
#include "arena.h"
#include "paddle.h"

class ball {
    public:
    ball(vec2 position, vec2 velocity) : position(position), velocity(velocity){};

    void renderBall(){
        mvaddch(lround(position.gety()), lround(position.getx()), 'o');
    }

    void spawnBall(ArenaBorder arena){
        setPos(vec2((arena.getW()/2), (arena.getH()/2)));
        setVel(vec2::randomUV());
    }

    void ballHitV(ArenaBorder arena){
       if (position.gety() >= arena.getH()) {setVel(vec2(velocity.getx(),-velocity.gety()));}
    }
    
    void paddleHit(paddle p){
        bool atPaddleX = (position.getx() == p.getPosX());
        bool inPaddleY = (position.gety() >= p.getPosY()) && (position.gety() <= p.getPosY() + p.getSize());
        if (atPaddleX && inPaddleY) {setVel(vec2(-velocity.getx(), velocity.gety()));}
    }

    void sideHit(ArenaBorder arena){
        if (position.getx() == arena.getW()) {setVel(vec2(-velocity.getx(), velocity.gety()));}
    }

    void tickPos(){
        position += velocity;
    }

    void setPos(vec2 pos){
        position = pos;
    }

    void setVel(vec2 vel){
        velocity = vel;
    }

    void Update(ArenaBorder arena, paddle p1, paddle p2){
        ballHitV(arena);
        sideHit(arena);
        paddleHit(p1);
        paddleHit(p2);
        tickPos();

    }

    private:
    vec2 position;
    vec2 velocity;
};

#endif