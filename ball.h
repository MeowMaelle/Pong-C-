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
        vec2 v = vec2::randomUV();
        v /= 3.0;                 // slow the ball down to a third of unit speed
        setVel(v);
    }

    // Bounce off the top and bottom walls (flip the vertical velocity).
    // Only reflect when actually heading INTO the wall, so the ball can't
    // get stuck flipping back and forth on the boundary.
    void wallHitV(ArenaBorder arena){
        double top    = 1;
        double bottom = arena.getH() - 2;   // last playable row (border is at getH()-1)
        if (position.gety() <= top && velocity.gety() < 0) {
            setVel(vec2(velocity.getx(), -velocity.gety()));
            position.sety(top);             // clamp back inside
        } else if (position.gety() >= bottom && velocity.gety() > 0) {
            setVel(vec2(velocity.getx(), -velocity.gety()));
            position.sety(bottom);
        }
    }

    // Bounce off the left and right walls (flip the horizontal velocity).
    void sideHit(ArenaBorder arena){
        double left  = 1;
        double right = arena.getW() - 2;    // last playable col (border is at getW()-1)
        if (position.getx() <= left && velocity.getx() < 0) {
            setVel(vec2(-velocity.getx(), velocity.gety()));
            position.setx(left);
        } else if (position.getx() >= right && velocity.getx() > 0) {
            setVel(vec2(-velocity.getx(), velocity.gety()));
            position.setx(right);
        }
    }

    void paddleHit(paddle p){
        bool atPaddleX = std::abs(position.getx() - p.getPosX()) < 1.0;
        bool inPaddleY = (position.gety() >= p.getPosY()) && (position.gety() <= p.getPosY() + p.getSize());
        if (atPaddleX && inPaddleY) {
            // always send the ball AWAY from the paddle, regardless of the
            // incoming sign, so a glancing hit can't re-trigger next frame
            double dir = (position.getx() >= p.getPosX()) ? 1.0 : -1.0;
            setVel(vec2(dir * std::abs(velocity.getx()), velocity.gety()));
        }
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
        wallHitV(arena);
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