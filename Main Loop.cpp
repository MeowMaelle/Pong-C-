#include <iostream>
#include "arena.h"
#include "paddle.h"
#include "ball.h"
#include "vec2.h"
using namespace std;

int arenaWidth = 60;
int arenaHeight = 20;

ArenaBorder arena(arenaHeight, arenaWidth);
paddle p1(2, vec2(1, 1));
paddle p2(2, vec2(58, 1));
bool quit = false;
void tickInput(paddle& P1, paddle& P2);

int main(){

    initscr();
    noecho(); // Stope Typed Keyes Showing on the screen as text
    cbreak(); // keys register istantly (they dont wait for me to click enter)
    keypad(stdscr, TRUE); // arrow keys register as their own code rather than a weird set of chars
    curs_set(0);
    nodelay(stdscr, TRUE);
    while (not quit){
        erase();
        tickInput(p1, p2);
        arena.renderArena();
        p1.renderPaddle();
        p2.renderPaddle();
        refresh();
        napms(16);
    }
    endwin();
    return 0;
}

void tickInput(paddle& P1, paddle& P2){
    int p1dir = 0;   // -1 = up, +1 = down, 0 = nothing this frame
    int p2dir = 0;
    int chr;
    while ((chr = getch()) != ERR){      // drain the ENTIRE buffer
        if      (chr == 'w')       p1dir = -1;   // just record intent...
        else if (chr == 's')       p1dir = 1;
        else if (chr == KEY_UP)    p2dir = -1;
        else if (chr == KEY_DOWN)  p2dir = 1;
        else if (chr == 'q')       quit = true;
    }
    if (p1dir) P1.movePaddle(p1dir, arena);     // ...then move once, after the buffer's empty
    if (p2dir) P2.movePaddle(p2dir, arena);
}