#include <iostream>
#include "arena.h"
#include "paddle.h"
#include "ball.h"
#include "vec2.h"
using namespace std;

ArenaBorder arena(10, 50);

int main(){

    initscr();
    noecho(); // Stope Typed Keyes Showing on the screen as text
    cbreak(); // keys register istantly (they dont wait for me to click enter)
    keypad(stdscr, TRUE); // arrow keys register as their own code rather than a weird set of chars
    curs_set(0);
    while (true){
        arena.renderArena();
        refresh();
        erase();
        getch();
        timeout(16)
    }
    endwin();
    return 0;
}


