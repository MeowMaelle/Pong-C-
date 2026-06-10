# ClaudeTerminalPong 🏓

A small but polished **Pong** for your terminal — written in modern C++17 with
**ncurses**. Two paddles, real ball physics with paddle "english", a scoreboard
rendered in chunky block digits, a CPU opponent with three difficulty levels,
and a clean menu / pause / game-over flow.

```
              ┌───────────────  P O N G  ───────────────┐
              │   ██   ██                        ██      │
              │   ██   ██     score band         ██      │
              ├──────────────────┊──────────────────────┤
              │ █                ┊                       │
              │ █                ┊            ●          │
              │ █   left paddle  ┊                  █    │
              │                  ┊                  █    │
              │                  ┊       right paddle    │
              └──────────────────────────────────────────┘
```

## Features

- **Two paddles** — local 1-player (vs CPU) and 2-player modes.
- **Ball physics** — the bounce angle depends on *where* the ball strikes the
  paddle, the rally speeds the ball up on every hit (up to a cap), and the
  vertical motion is scaled for the terminal's tall cells so trajectories look
  natural and stay biased horizontal.
- **Scoring & win condition** — first to **5 / 7 / 11** (selectable).
- **CPU opponent** — Easy / Medium / Hard; Hard predicts the ball's landing row
  by simulating wall bounces.
- **Aesthetics** — colored border, dashed center net, big block-digit scoreboard,
  a 3-2-1 serve countdown, a border flash on each point, and a figlet title.
- **Robust** — delta-time updates (frame-rate independent), live terminal-resize
  handling, a "too small" guard, and guaranteed terminal restore on exit.

## Build & run

Requires a C++17 compiler and ncurses (wide-character `ncursesw` preferred).

```sh
# from this directory
make
./pong          # or: make run
```

On Debian/Ubuntu you can get the dependencies with:

```sh
sudo apt-get install build-essential libncurses-dev
```

### CMake (alternative)

```sh
cmake -S . -B build
cmake --build build
./build/pong
```

## Controls

| Action            | Player 1        | Player 2 (2P mode) |
|-------------------|-----------------|--------------------|
| Move up / down    | `W` / `S`       | `↑` / `↓`          |

In 1-player mode you may use **either** `W`/`S` **or** the arrow keys.

| Key              | Effect                                  |
|------------------|-----------------------------------------|
| `↑` / `↓`        | Menu: move selection                    |
| `←` / `→`        | Menu: change the highlighted setting    |
| `Enter`          | Menu: select • Game over: rematch       |
| `P` / `Space`    | Pause / resume                          |
| `M`              | Game over: back to menu                 |
| `Q`              | In game: back to menu • In menu: quit   |

## Project layout

```
ClaudeTerminalPong/
├── Makefile            # primary build
├── CMakeLists.txt      # alternative build
├── README.md
└── src/
    ├── main.cpp        # ncurses session + main loop entry
    ├── game.h / .cpp   # game state, physics, AI, rendering
    ├── config.h        # all tunable constants
    ├── digits.h        # block-digit font + title banner
    └── vec2.h          # tiny 2D vector helper
```

Tweak the feel in [`src/config.h`](src/config.h) — ball speed, paddle speed,
spin, AI profiles, and target scores all live there.
