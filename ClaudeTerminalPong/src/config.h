#pragma once

// ---------------------------------------------------------------------------
// Tunable gameplay / presentation constants. All speeds are in terminal cells
// per second; the update loop is delta-time based so behaviour is independent
// of frame rate.
// ---------------------------------------------------------------------------
namespace cfg {

// Frame timing -------------------------------------------------------------
constexpr int    TARGET_FPS   = 60;
constexpr double FRAME_TIME   = 1.0 / TARGET_FPS;
constexpr double MAX_DELTA    = 0.05;   // clamp dt so a stall can't teleport the ball

// Ball ---------------------------------------------------------------------
constexpr float  BALL_SPEED      = 24.0f;  // serve speed (horizontal)
constexpr float  BALL_MAX_SPEED  = 60.0f;  // cap after rally speed-ups
constexpr float  BALL_SPEEDUP    = 1.05f;  // multiplier per paddle hit
// Terminal cells are roughly twice as tall as wide, so a smaller vertical
// factor keeps trajectories looking natural (and biased horizontal).
constexpr float  BALL_VY_SCALE   = 0.55f;  // max |vy| as fraction of |vx| on a paddle hit
constexpr float  SERVE_VY_SCALE  = 0.35f;  // vertical spread of the opening serve

// Paddle -------------------------------------------------------------------
constexpr int    PADDLE_HEIGHT = 5;        // keep odd so it has a true centre row
constexpr float  PADDLE_SPEED  = 30.0f;
constexpr float  INPUT_HOLD    = 0.13f;    // seconds of glide granted per key event

// Serve / countdown pauses -------------------------------------------------
constexpr float  COUNTDOWN_START = 3.0f;   // "3..2..1" at match start
constexpr float  COUNTDOWN_POINT = 1.3f;   // brief pause after a point
constexpr float  SCORE_FLASH     = 0.18f;  // border flash duration on a score

// AI per-difficulty: {speed multiplier, reaction dead-zone in cells, predicts?}
struct AiProfile { float speed; float deadzone; bool predict; };
constexpr AiProfile AI_EASY   { 0.55f, 2.2f, false };
constexpr AiProfile AI_MEDIUM { 0.82f, 1.1f, false };
constexpr AiProfile AI_HARD   { 1.05f, 0.5f, true  };

// Layout -------------------------------------------------------------------
constexpr int    SCORE_TOP   = 1;          // first row of the big score digits
constexpr int    DIVIDER_ROW = 6;          // horizontal rule under the score band
constexpr int    PLAY_TOP    = 7;          // first playfield row
constexpr int    PADDLE_INSET = 2;         // columns between wall and paddle
constexpr int    MIN_COLS    = 48;
constexpr int    MIN_ROWS    = 18;

} // namespace cfg
