#include <iostream>
#include "arena.h"
#include "paddle.h"
#include "ball.h"
#include "vec2.h"
using namespace std;

ArenaBorder arena(10, 50);
paddle p1(2, 1);
paddle p2(2,1);
void tickInput(paddle& P1, paddle& P2);

int main(){

    initscr();
    noecho(); // Stope Typed Keyes Showing on the screen as text
    cbreak(); // keys register istantly (they dont wait for me to click enter)
    keypad(stdscr, TRUE); // arrow keys register as their own code rather than a weird set of chars
    curs_set(0);
    arena.renderArena();
    while (true){
        refresh();
        erase();
        tickInput(p1, p2);
        timeout(16);
    }
    endwin();
    return 0;
}

void tickInput(paddle& P1, paddle& P2){
    bool P2move = false;
    bool P1move = false;
    while ((getch() != ERR) || not(P2move && P1move)){
        if ((char(getch()) == 'w') && not P1move){
            P1.movePaddle(1);
            P1move = true;
        }
        if ((char(getch()) == 's') && not P1move){
            P1.movePaddle(-1);
            P1move = true;
        }
        if ((getch() == KEY_UP) && not P2move){
            P2.movePaddle(1);
            P2move = true;
        }
        if ((getch() == KEY_DOWN) && not P2move){
            P2.movePaddle(-1);
            P2move = true;
        }
    }
    while (getch() != ERR)
    {
        auto temp = getch();
    }
    
}