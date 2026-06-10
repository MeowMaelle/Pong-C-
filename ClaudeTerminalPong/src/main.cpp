// ClaudeTerminalPong — a small, polished Pong for the terminal (ncurses, C++17).
//
//   Build:  make           (from the ClaudeTerminalPong directory)
//   Play:   ./pong   (or  make run)
//
// Controls are shown on screen. Quit any time with Q.

#include <clocale>
#include <cstdio>
#include <exception>

#include <ncurses.h>

#include "game.h"

// RAII guard so the terminal is always restored, even if something throws.
struct CursesSession {
    CursesSession() {
        initscr();
        cbreak();                 // raw-ish: deliver keys immediately, no line buffering
        noecho();                 // don't echo typed keys
        keypad(stdscr, TRUE);     // decode arrow keys, KEY_RESIZE, etc.
        nodelay(stdscr, TRUE);    // non-blocking getch()
        curs_set(0);              // hide the cursor
        set_escdelay(25);         // snappier Esc handling
    }
    ~CursesSession() { endwin(); }
};

int main() {
    std::setlocale(LC_ALL, ""); // honour the user's UTF-8 locale for box/block glyphs

    try {
        CursesSession session;
        Game game;
        game.run();
    } catch (const std::exception& e) {
        // CursesSession's destructor has already restored the terminal here.
        std::fprintf(stderr, "ClaudeTerminalPong error: %s\n", e.what());
        return 1;
    }
    return 0;
}
