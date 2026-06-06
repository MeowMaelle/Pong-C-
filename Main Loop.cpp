#include <iostream>
#include "arena.h"
#include "paddle.h"
#include "ball.h"
#include "vec2.h"
using namespace std;

ArenaBorder arena(10, 60);
paddle p1(2, vec2(59, 1));
paddle p2(2, vec2(1, 1));
bool quit = false;
void tickInput(paddle& P1, paddle& P2);

int main(){

    initscr();
    noecho(); // Stope Typed Keyes Showing on the screen as text
    cbreak(); // keys register istantly (they dont wait for me to click enter)
    keypad(stdscr, TRUE); // arrow keys register as their own code rather than a weird set of chars
    curs_set(0);
    timeout(200);
    while (not quit){
        erase();
        arena.renderArena();
        p1.renderPaddle();
        p2.renderPaddle();
        tickInput(p1, p2);
        refresh();
    }
    endwin();
    return 0;
}

void tickInput(paddle& P1, paddle& P2){
    int chr;
    while ((chr = getch()) != ERR){
        if (chr == 'w') {P1.movePaddle(1);}
        if (chr == 's') {P1.movePaddle(-1);}
        if (chr == KEY_UP) {P2.movePaddle(1);}
        if (chr == KEY_DOWN) {P2.movePaddle(-1);}
        if (chr == 'q') {quit = true;}
    }
}