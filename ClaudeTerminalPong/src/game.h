#pragma once

#include <random>
#include "config.h"
#include "vec2.h"

// Colour pair identifiers (initialised in Game::initColors).
enum ColorPair {
    CP_DEFAULT = 0,
    CP_BORDER  = 1,
    CP_NET     = 2,
    CP_BALL    = 3,
    CP_LEFT    = 4,   // left paddle + P1 score
    CP_RIGHT   = 5,   // right paddle + P2/CPU score
    CP_TITLE   = 6,
    CP_HINT    = 7,
    CP_HILITE  = 8,   // highlighted menu row
    CP_FLASH   = 9,   // border flash on a score
    CP_WIN     = 10,
};

enum class State  { Menu, Playing, Paused, GameOver };
enum class Mode   { OnePlayer, TwoPlayer };
enum class Diff   { Easy, Medium, Hard };
enum class Winner { None, Left, Right };

// Playfield geometry, recomputed every frame from the current terminal size.
struct Geo {
    int rows = 0, cols = 0;
    int top = 0, bottom = 0, left = 0, right = 0; // inclusive playfield bounds
    int dividerRow = 0;
    int centerCol = 0;
    int leftPaddleX = 0, rightPaddleX = 0;
    bool tooSmall = false;
};

struct Paddle {
    float y      = 0.0f;   // centre row (float for smooth motion)
    int   dir    = 0;      // -1 up, +1 down, 0 idle
    float hold   = 0.0f;   // remaining glide time from the last key event
    int   score  = 0;
};

struct Ball {
    float x = 0.0f, y = 0.0f;
    float vx = 0.0f, vy = 0.0f;
};

class Game {
public:
    Game();
    void run();              // top-level loop: menu -> match -> menu ...

private:
    // --- setup ---
    void initColors();

    // --- input ---
    void processInput();
    void onMenuKey(int ch);
    void onPlayingKey(int ch);
    void onPausedKey(int ch);
    void onGameOverKey(int ch);
    void adjustMenu(int delta);            // change the value on the selected menu row

    // --- update ---
    void update(double dt);
    void updatePlaying(double dt);
    void movePaddle(Paddle& p, double dt);
    void aiMove(double dt);
    void moveBall(double dt);
    float predictBallY() const;            // where the ball will cross the AI paddle plane

    // --- match flow ---
    void startMatch();                     // reset scores, begin countdown
    void resetBall(int towardDir, float countdown);
    void launchServe();
    int  targetScoreValue() const;

    // --- render ---
    void render();
    void renderMenu();
    void renderPlaying();
    void renderPaused();
    void renderGameOver();

    void drawBorder(int colorPair);
    void drawScoreBand();
    void drawNet();
    void drawPaddle(int col, const Paddle& p, int colorPair);
    void drawBall();
    void drawBigDigit(int value, int row, int col); // 0-9, paints filled cells with colour-on caller
    void drawNumber(int value, int row, int anchorX, bool rightAlign, int colorPair);
    void drawCountdown();
    void drawHint();                       // controls strip in the bottom border

    // --- helpers ---
    void computeGeo();
    float randf(float lo, float hi);
    int   randSign();

    // --- state ---
    State  state   = State::Menu;
    Mode   mode    = Mode::OnePlayer;
    Diff   diff    = Diff::Medium;
    int    targetIdx = 1;          // index into TARGETS (5 / 7 / 11)
    int    menuIndex = 0;
    Winner winner  = Winner::None;
    bool   running = true;

    Paddle pL, pR;
    Ball   ball;
    Geo    geo;

    float  freezeTimer = 0.0f;     // >0 means ball is parked for the serve countdown
    bool   serveArmed  = false;    // velocity still needs to be set when the freeze ends
    int    serveDir    = -1;       // -1 serve toward left, +1 toward right
    float  flashTimer  = 0.0f;     // border score-flash timer

    std::mt19937 rng;
};
