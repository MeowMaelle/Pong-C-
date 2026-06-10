#include "game.h"

#include <ncurses.h>

#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>
#include <thread>

#include "digits.h"

// UTF-8 glyphs. The terminal must be in a UTF-8 locale (set in main()).
static const char* const BLOCK = "█"; // █  paddles, score digits, net
static const char* const BALL  = "●"; // ●  the ball

// ---------------------------------------------------------------------------
// Small text helpers
// ---------------------------------------------------------------------------

// Display width of a UTF-8 string, counting each code point (all glyphs we use
// are single-column) so centring stays correct with box-drawing characters.
static int uwidth(const char* s) {
    int w = 0;
    for (; *s; ++s)
        if ((static_cast<unsigned char>(*s) & 0xC0) != 0x80) ++w;
    return w;
}

static void putCentered(int row, int cols, const char* text) {
    int col = (cols - uwidth(text)) / 2;
    if (col < 0) col = 0;
    mvaddstr(row, col, text);
}

// ---------------------------------------------------------------------------
// Construction / setup
// ---------------------------------------------------------------------------

Game::Game() {
    std::random_device rd;
    rng.seed(rd());
}

void Game::initColors() {
    if (!has_colors()) return;
    start_color();
    use_default_colors(); // allow -1 == terminal's default background
    init_pair(CP_BORDER, COLOR_BLUE,    -1);
    init_pair(CP_NET,    COLOR_WHITE,   -1);
    init_pair(CP_BALL,   COLOR_YELLOW,  -1);
    init_pair(CP_LEFT,   COLOR_CYAN,    -1);
    init_pair(CP_RIGHT,  COLOR_MAGENTA, -1);
    init_pair(CP_TITLE,  COLOR_GREEN,   -1);
    init_pair(CP_HINT,   COLOR_WHITE,   -1);
    init_pair(CP_HILITE, COLOR_BLACK,   COLOR_CYAN);
    init_pair(CP_FLASH,  COLOR_YELLOW,  -1);
    init_pair(CP_WIN,    COLOR_BLACK,   COLOR_GREEN);
}

// ---------------------------------------------------------------------------
// Random helpers
// ---------------------------------------------------------------------------

float Game::randf(float lo, float hi) {
    std::uniform_real_distribution<float> d(lo, hi);
    return d(rng);
}

int Game::randSign() { return (rng() & 1u) ? 1 : -1; }

// ---------------------------------------------------------------------------
// Geometry
// ---------------------------------------------------------------------------

void Game::computeGeo() {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    geo.rows = rows;
    geo.cols = cols;
    geo.tooSmall = (rows < cfg::MIN_ROWS || cols < cfg::MIN_COLS);
    geo.dividerRow   = cfg::DIVIDER_ROW;
    geo.top          = cfg::PLAY_TOP;
    geo.bottom       = rows - 2;
    geo.left         = 1;
    geo.right        = cols - 2;
    geo.centerCol    = cols / 2;
    geo.leftPaddleX  = geo.left + cfg::PADDLE_INSET;
    geo.rightPaddleX = geo.right - cfg::PADDLE_INSET;
}

int Game::targetScoreValue() const {
    static const int T[3] = {5, 7, 11};
    return T[targetIdx];
}

// ---------------------------------------------------------------------------
// Main loop
// ---------------------------------------------------------------------------

void Game::run() {
    initColors();
    using clock = std::chrono::steady_clock;
    auto prev = clock::now();

    while (running) {
        auto now = clock::now();
        double dt = std::chrono::duration<double>(now - prev).count();
        prev = now;
        if (dt > cfg::MAX_DELTA) dt = cfg::MAX_DELTA;

        computeGeo();
        processInput();
        update(dt);
        render();

        // Cap the frame rate so we are not spinning a CPU core.
        double spent = std::chrono::duration<double>(clock::now() - now).count();
        double rest = cfg::FRAME_TIME - spent;
        if (rest > 0)
            std::this_thread::sleep_for(std::chrono::duration<double>(rest));
    }
}

// ---------------------------------------------------------------------------
// Input
// ---------------------------------------------------------------------------

void Game::processInput() {
    int ch;
    while ((ch = getch()) != ERR) {
        if (ch == KEY_RESIZE) { clear(); continue; }
        switch (state) {
            case State::Menu:     onMenuKey(ch);     break;
            case State::Playing:  onPlayingKey(ch);  break;
            case State::Paused:   onPausedKey(ch);   break;
            case State::GameOver: onGameOverKey(ch); break;
        }
        if (!running) break;
    }
}

void Game::adjustMenu(int delta) {
    switch (menuIndex) {
        case 0: // mode toggles regardless of direction
            mode = (mode == Mode::OnePlayer) ? Mode::TwoPlayer : Mode::OnePlayer;
            break;
        case 1: { // difficulty cycles Easy/Medium/Hard
            int d = ((static_cast<int>(diff) + delta) % 3 + 3) % 3;
            diff = static_cast<Diff>(d);
            break;
        }
        case 2: // target score 5 / 7 / 11
            targetIdx = ((targetIdx + delta) % 3 + 3) % 3;
            break;
        default:
            break;
    }
}

void Game::onMenuKey(int ch) {
    const int ITEMS = 5; // 0 mode, 1 difficulty, 2 target, 3 start, 4 quit
    switch (ch) {
        case KEY_UP: case 'w': case 'W':
            menuIndex = (menuIndex + ITEMS - 1) % ITEMS; break;
        case KEY_DOWN: case 's': case 'S':
            menuIndex = (menuIndex + 1) % ITEMS; break;
        case KEY_LEFT: case 'a': case 'A':
            adjustMenu(-1); break;
        case KEY_RIGHT: case 'd': case 'D':
            adjustMenu(+1); break;
        case '\n': case '\r': case KEY_ENTER:
            if (menuIndex == 3)      startMatch();
            else if (menuIndex == 4) running = false;
            else                     adjustMenu(+1);
            break;
        case 'q': case 'Q':
            running = false; break;
        default: break;
    }
}

void Game::onPlayingKey(int ch) {
    switch (ch) {
        case 'w': case 'W': pL.dir = -1; pL.hold = cfg::INPUT_HOLD; break;
        case 's': case 'S': pL.dir = +1; pL.hold = cfg::INPUT_HOLD; break;
        case KEY_UP:
            if (mode == Mode::TwoPlayer) { pR.dir = -1; pR.hold = cfg::INPUT_HOLD; }
            else                         { pL.dir = -1; pL.hold = cfg::INPUT_HOLD; }
            break;
        case KEY_DOWN:
            if (mode == Mode::TwoPlayer) { pR.dir = +1; pR.hold = cfg::INPUT_HOLD; }
            else                         { pL.dir = +1; pL.hold = cfg::INPUT_HOLD; }
            break;
        case 'p': case 'P': case ' ': state = State::Paused; break;
        case 'q': case 'Q': state = State::Menu;   break;
        default: break;
    }
}

void Game::onPausedKey(int ch) {
    switch (ch) {
        case 'p': case 'P': case ' ': state = State::Playing; break;
        case 'q': case 'Q':           state = State::Menu;    break;
        default: break;
    }
}

void Game::onGameOverKey(int ch) {
    switch (ch) {
        case '\n': case '\r': case KEY_ENTER: startMatch();        break;
        case 'm': case 'M':                   state = State::Menu; break;
        case 'q': case 'Q':                   running = false;     break;
        default: break;
    }
}

// ---------------------------------------------------------------------------
// Match flow
// ---------------------------------------------------------------------------

void Game::startMatch() {
    pL.score = pR.score = 0;
    pL.dir = pR.dir = 0;
    pL.hold = pR.hold = 0.0f;
    float midY = (geo.top + geo.bottom) / 2.0f;
    pL.y = pR.y = midY;
    winner = Winner::None;
    flashTimer = 0.0f;
    state = State::Playing;
    resetBall(randSign(), cfg::COUNTDOWN_START);
}

void Game::resetBall(int towardDir, float countdown) {
    ball.x = (geo.left + geo.right) / 2.0f;
    ball.y = (geo.top + geo.bottom) / 2.0f;
    ball.vx = ball.vy = 0.0f;
    serveDir = towardDir;
    freezeTimer = countdown;
    serveArmed = true;
}

void Game::launchServe() {
    ball.vx = serveDir * cfg::BALL_SPEED;
    ball.vy = cfg::BALL_SPEED * cfg::SERVE_VY_SCALE * randf(-1.0f, 1.0f);
    serveArmed = false;
}

// ---------------------------------------------------------------------------
// Update
// ---------------------------------------------------------------------------

void Game::update(double dt) {
    if (flashTimer > 0.0f) {
        flashTimer -= static_cast<float>(dt);
        if (flashTimer < 0.0f) flashTimer = 0.0f;
    }
    if (geo.tooSmall) return;            // freeze everything until the window is usable
    if (state == State::Playing) updatePlaying(dt);
}

void Game::updatePlaying(double dt) {
    movePaddle(pL, dt);
    if (mode == Mode::TwoPlayer) movePaddle(pR, dt);
    else                         aiMove(dt);

    if (freezeTimer > 0.0f) {
        freezeTimer -= static_cast<float>(dt);
        // Park the ball at centre while the serve counts down.
        ball.x = (geo.left + geo.right) / 2.0f;
        ball.y = (geo.top + geo.bottom) / 2.0f;
        if (freezeTimer <= 0.0f) {
            freezeTimer = 0.0f;
            if (serveArmed) launchServe();
        }
        return;
    }
    moveBall(dt);
}

void Game::movePaddle(Paddle& p, double dt) {
    if (p.hold > 0.0f) {
        p.y += p.dir * cfg::PADDLE_SPEED * static_cast<float>(dt);
        p.hold -= static_cast<float>(dt);
        if (p.hold <= 0.0f) { p.hold = 0.0f; p.dir = 0; }
    }
    const float lo = geo.top    + cfg::PADDLE_HEIGHT / 2;
    const float hi = geo.bottom - cfg::PADDLE_HEIGHT / 2;
    if (p.y < lo) p.y = lo;
    if (p.y > hi) p.y = hi;
}

void Game::aiMove(double dt) {
    cfg::AiProfile prof = (diff == Diff::Easy) ? cfg::AI_EASY
                        : (diff == Diff::Hard) ? cfg::AI_HARD
                                               : cfg::AI_MEDIUM;
    float target;
    if (ball.vx > 0.0f)                       // ball heading toward the AI (right side)
        target = prof.predict ? predictBallY() : ball.y;
    else                                      // otherwise drift back to centre
        target = (geo.top + geo.bottom) / 2.0f;

    float d = target - pR.y;
    if (std::fabs(d) > prof.deadzone) {
        float step = prof.speed * cfg::PADDLE_SPEED * static_cast<float>(dt);
        if (step > std::fabs(d)) step = std::fabs(d);
        pR.y += (d > 0.0f ? 1.0f : -1.0f) * step;
    }
    const float lo = geo.top    + cfg::PADDLE_HEIGHT / 2;
    const float hi = geo.bottom - cfg::PADDLE_HEIGHT / 2;
    if (pR.y < lo) pR.y = lo;
    if (pR.y > hi) pR.y = hi;
}

float Game::predictBallY() const {
    if (ball.vx <= 0.0f) return (geo.top + geo.bottom) / 2.0f;
    float t = (geo.rightPaddleX - ball.x) / ball.vx;
    if (t < 0.0f) return ball.y;
    float yEnd = ball.y + ball.vy * t;
    float span = static_cast<float>(geo.bottom - geo.top);
    if (span <= 0.0f) return ball.y;
    // Fold the landing point into [top, bottom] as a triangle wave (wall bounces).
    float m = std::fmod(yEnd - geo.top, 2.0f * span);
    if (m < 0.0f) m += 2.0f * span;
    if (m > span) m = 2.0f * span - m;
    return geo.top + m;
}

void Game::moveBall(double dt) {
    float nx = ball.x + ball.vx * static_cast<float>(dt);
    float ny = ball.y + ball.vy * static_cast<float>(dt);

    // Top / bottom walls: reflect position and velocity.
    if (ny < geo.top)    { ny = geo.top + (geo.top - ny);    ball.vy = -ball.vy; }
    if (ny > geo.bottom) { ny = geo.bottom - (ny - geo.bottom); ball.vy = -ball.vy; }

    const float Lx = static_cast<float>(geo.leftPaddleX);
    const float Rx = static_cast<float>(geo.rightPaddleX);
    const float half = cfg::PADDLE_HEIGHT / 2.0f;

    // Bounce off a paddle: reflect, add "english" from the contact point, speed up.
    auto bounce = [&](const Paddle& p, int dir, float hitY) {
        float offset = (hitY - p.y) / (half + 0.5f);
        if (offset < -1.0f) offset = -1.0f;
        if (offset >  1.0f) offset =  1.0f;
        float speed = std::fabs(ball.vx) * cfg::BALL_SPEEDUP;
        if (speed < cfg::BALL_SPEED)     speed = cfg::BALL_SPEED;
        if (speed > cfg::BALL_MAX_SPEED) speed = cfg::BALL_MAX_SPEED;
        ball.vx = dir * speed;
        ball.vy = offset * speed * cfg::BALL_VY_SCALE;
    };

    if (ball.vx < 0.0f && nx <= Lx && ball.x >= Lx) {
        if (ny >= pL.y - half - 0.5f && ny <= pL.y + half + 0.5f) {
            bounce(pL, +1, ny);
            nx = Lx + (Lx - nx);          // reflect the leftover travel back into play
        }
    } else if (ball.vx > 0.0f && nx >= Rx && ball.x <= Rx) {
        if (ny >= pR.y - half - 0.5f && ny <= pR.y + half + 0.5f) {
            bounce(pR, -1, ny);
            nx = Rx - (nx - Rx);
        }
    }

    ball.x = nx;
    ball.y = ny;

    // Scoring: a ball that slips past a paddle and reaches the wall.
    if (ball.x < geo.left) {
        pR.score++;
        flashTimer = cfg::SCORE_FLASH;
        if (pR.score >= targetScoreValue()) { winner = Winner::Right; state = State::GameOver; }
        else resetBall(-1, cfg::COUNTDOWN_POINT); // serve toward the player who conceded
    } else if (ball.x > geo.right) {
        pL.score++;
        flashTimer = cfg::SCORE_FLASH;
        if (pL.score >= targetScoreValue()) { winner = Winner::Left; state = State::GameOver; }
        else resetBall(+1, cfg::COUNTDOWN_POINT);
    }
}

// ---------------------------------------------------------------------------
// Rendering
// ---------------------------------------------------------------------------

void Game::render() {
    erase();

    if (geo.tooSmall) {
        attron(COLOR_PAIR(CP_HINT) | A_BOLD);
        putCentered(geo.rows / 2, geo.cols, "Terminal too small");
        attroff(A_BOLD);
        char buf[64];
        std::snprintf(buf, sizeof buf, "Please resize to at least %d x %d",
                      cfg::MIN_COLS, cfg::MIN_ROWS);
        putCentered(geo.rows / 2 + 1, geo.cols, buf);
        attroff(COLOR_PAIR(CP_HINT));
        refresh();
        return;
    }

    switch (state) {
        case State::Menu:
            renderMenu();
            break;
        case State::Playing:
            renderPlaying();
            break;
        case State::Paused:
            renderPlaying();
            renderPaused();
            break;
        case State::GameOver:
            renderPlaying();
            renderGameOver();
            break;
    }
    refresh();
}

void Game::drawBorder(int cp) {
    attron(COLOR_PAIR(cp));
    box(stdscr, 0, 0);
    mvhline(geo.dividerRow, 1, ACS_HLINE, geo.cols - 2);
    mvaddch(geo.dividerRow, 0, ACS_LTEE);
    mvaddch(geo.dividerRow, geo.cols - 1, ACS_RTEE);
    attroff(COLOR_PAIR(cp));

    attron(COLOR_PAIR(CP_TITLE) | A_BOLD);
    putCentered(0, geo.cols, " P O N G ");
    attroff(COLOR_PAIR(CP_TITLE) | A_BOLD);
}

void Game::drawBigDigit(int value, int row, int col) {
    if (value < 0 || value > 9) return;
    for (int r = 0; r < font::DIGIT_ROWS; ++r) {
        const char* pat = font::DIGITS[value][r];
        for (int c = 0; c < font::DIGIT_COLS; ++c) {
            if (pat[c] == '#') {
                int x = col + c * font::CELL_W;
                mvaddstr(row + r, x,     BLOCK);
                mvaddstr(row + r, x + 1, BLOCK);
            }
        }
    }
}

void Game::drawNumber(int value, int row, int anchorX, bool rightAlign, int cp) {
    std::string s = std::to_string(value);
    int n = static_cast<int>(s.size());
    int width = n * font::DIGIT_W + (n - 1) * font::DIGIT_GAP;
    int startCol = rightAlign ? anchorX - width : anchorX;

    attron(COLOR_PAIR(cp) | A_BOLD);
    int col = startCol;
    for (char c : s) {
        drawBigDigit(c - '0', row, col);
        col += font::DIGIT_W + font::DIGIT_GAP;
    }
    attroff(COLOR_PAIR(cp) | A_BOLD);
}

void Game::drawScoreBand() {
    drawNumber(pL.score, cfg::SCORE_TOP, geo.centerCol - 3, true,  CP_LEFT);
    drawNumber(pR.score, cfg::SCORE_TOP, geo.centerCol + 3, false, CP_RIGHT);
}

void Game::drawNet() {
    attron(COLOR_PAIR(CP_NET) | A_DIM);
    for (int r = geo.top; r <= geo.bottom; ++r)
        if (((r - geo.top) % 2) == 0)
            mvaddstr(r, geo.centerCol, BLOCK);
    attroff(COLOR_PAIR(CP_NET) | A_DIM);
}

void Game::drawPaddle(int col, const Paddle& p, int cp) {
    int cy = static_cast<int>(std::lround(p.y));
    int half = cfg::PADDLE_HEIGHT / 2;
    attron(COLOR_PAIR(cp) | A_BOLD);
    for (int i = -half; i <= half; ++i) {
        int r = cy + i;
        if (r >= geo.top && r <= geo.bottom)
            mvaddstr(r, col, BLOCK);
    }
    attroff(COLOR_PAIR(cp) | A_BOLD);
}

void Game::drawBall() {
    int bx = static_cast<int>(std::lround(ball.x));
    int by = static_cast<int>(std::lround(ball.y));
    if (bx < geo.left)   bx = geo.left;
    if (bx > geo.right)  bx = geo.right;
    if (by < geo.top)    by = geo.top;
    if (by > geo.bottom) by = geo.bottom;
    attron(COLOR_PAIR(CP_BALL) | A_BOLD);
    mvaddstr(by, bx, BALL);
    attroff(COLOR_PAIR(CP_BALL) | A_BOLD);
}

void Game::drawCountdown() {
    int n = static_cast<int>(std::ceil(freezeTimer));
    if (n < 1) n = 1;
    if (n > 9) n = 9;
    int row = (geo.top + geo.bottom) / 2 - font::DIGIT_ROWS / 2;
    int col = geo.centerCol - font::DIGIT_W / 2;
    attron(COLOR_PAIR(CP_BALL) | A_BOLD);
    drawBigDigit(n, row, col);
    attroff(COLOR_PAIR(CP_BALL) | A_BOLD);
}

void Game::drawHint() {
    const char* h = (mode == Mode::TwoPlayer)
        ? "P1 W/S    P2 ↑/↓    P pause    Q menu"
        : "Move ↑/↓ or W/S    P pause    Q menu";
    attron(COLOR_PAIR(CP_HINT) | A_DIM);
    putCentered(geo.rows - 1, geo.cols, h);
    attroff(COLOR_PAIR(CP_HINT) | A_DIM);
}

void Game::renderPlaying() {
    drawBorder(flashTimer > 0.0f ? CP_FLASH : CP_BORDER);
    drawScoreBand();
    drawNet();
    drawPaddle(geo.leftPaddleX,  pL, CP_LEFT);
    drawPaddle(geo.rightPaddleX, pR, CP_RIGHT);
    if (freezeTimer > 0.0f) drawCountdown();
    else                    drawBall();
    drawHint();
}

void Game::renderMenu() {
    drawBorder(CP_BORDER);

    int titleRow = 3;
    attron(COLOR_PAIR(CP_TITLE) | A_BOLD);
    for (int i = 0; i < font::TITLE_ROWS; ++i)
        putCentered(titleRow + i, geo.cols, font::TITLE[i]);
    attroff(COLOR_PAIR(CP_TITLE) | A_BOLD);

    attron(COLOR_PAIR(CP_HINT) | A_DIM);
    putCentered(titleRow + font::TITLE_ROWS, geo.cols, "T E R M I N A L   E D I T I O N");
    attroff(COLOR_PAIR(CP_HINT) | A_DIM);

    const char* modeStr = (mode == Mode::OnePlayer) ? "1 Player (vs CPU)" : "2 Players";
    const char* diffStr = (diff == Diff::Easy) ? "Easy"
                        : (diff == Diff::Hard) ? "Hard" : "Medium";

    std::string opts[5] = {
        std::string("Mode:        ") + modeStr,
        std::string("Difficulty:  ") + diffStr,
        std::string("First to:    ") + std::to_string(targetScoreValue()),
        std::string("START  GAME"),
        std::string("QUIT"),
    };

    int baseRow = titleRow + font::TITLE_ROWS + 2;
    for (int i = 0; i < 5; ++i) {
        int row = baseRow + i * 2;
        if (row >= geo.rows - 2) break; // keep clear of the border on tiny screens
        bool selected = (i == menuIndex);
        bool disabled = (i == 1 && mode == Mode::TwoPlayer); // difficulty unused in 2P

        std::string text = opts[i];
        if (selected) text = "►  " + text + "  ◄";

        int attr = A_BOLD;
        int cp = CP_HINT;
        if (selected) cp = CP_HILITE;
        if (disabled) attr = A_DIM;

        attron(COLOR_PAIR(cp) | attr);
        putCentered(row, geo.cols, text.c_str());
        attroff(COLOR_PAIR(cp) | attr);
    }

    attron(COLOR_PAIR(CP_HINT) | A_DIM);
    putCentered(geo.rows - 1, geo.cols,
                "↑/↓ choose    ←/→ change    Enter select    Q quit");
    attroff(COLOR_PAIR(CP_HINT) | A_DIM);
}

void Game::renderPaused() {
    int midR = (geo.top + geo.bottom) / 2;
    attron(COLOR_PAIR(CP_HILITE) | A_BOLD);
    putCentered(midR, geo.cols, "  PAUSED  ");
    attroff(COLOR_PAIR(CP_HILITE) | A_BOLD);
    attron(COLOR_PAIR(CP_HINT));
    putCentered(midR + 2, geo.cols, "P / Space resume     Q quit to menu");
    attroff(COLOR_PAIR(CP_HINT));
}

void Game::renderGameOver() {
    const char* who;
    if (winner == Winner::Left)
        who = (mode == Mode::OnePlayer) ? "  YOU WIN!  " : "  PLAYER 1 WINS!  ";
    else
        who = (mode == Mode::OnePlayer) ? "  CPU WINS!  " : "  PLAYER 2 WINS!  ";

    int midR = (geo.top + geo.bottom) / 2;

    attron(COLOR_PAIR(CP_WIN) | A_BOLD);
    putCentered(midR - 2, geo.cols, who);
    attroff(COLOR_PAIR(CP_WIN) | A_BOLD);

    char fs[64];
    std::snprintf(fs, sizeof fs, "Final Score    %d : %d", pL.score, pR.score);
    attron(COLOR_PAIR(CP_HINT) | A_BOLD);
    putCentered(midR, geo.cols, fs);
    attroff(COLOR_PAIR(CP_HINT) | A_BOLD);

    attron(COLOR_PAIR(CP_HINT) | A_DIM);
    putCentered(midR + 2, geo.cols, "Enter rematch     M menu     Q quit");
    attroff(COLOR_PAIR(CP_HINT) | A_DIM);
}
