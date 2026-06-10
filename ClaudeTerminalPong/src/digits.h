#pragma once

// 3x5 bitmap font for the big scoreboard digits. '#' marks a filled cell,
// which the renderer paints as a 2-wide block so each numeral is 6x5.
namespace font {

inline const char* const DIGITS[10][5] = {
    {"###", "# #", "# #", "# #", "###"}, // 0
    {" # ", "## ", " # ", " # ", "###"}, // 1
    {"###", "  #", "###", "#  ", "###"}, // 2
    {"###", "  #", "###", "  #", "###"}, // 3
    {"# #", "# #", "###", "  #", "  #"}, // 4
    {"###", "#  ", "###", "  #", "###"}, // 5
    {"###", "#  ", "###", "# #", "###"}, // 6
    {"###", "  #", "  #", "  #", "  #"}, // 7
    {"###", "# #", "###", "# #", "###"}, // 8
    {"###", "# #", "###", "  #", "###"}, // 9
};

inline constexpr int DIGIT_ROWS = 5;
inline constexpr int DIGIT_COLS = 3;
inline constexpr int CELL_W     = 2;            // each filled cell is 2 columns wide
inline constexpr int DIGIT_W    = DIGIT_COLS * CELL_W; // 6 columns per digit
inline constexpr int DIGIT_GAP  = 1;            // blank columns between digits

// Classic figlet "Standard" banner for the title screen.
inline const char* const TITLE[5] = {
    " ____   ___  _   _  ____ ",
    "|  _ \\ / _ \\| \\ | |/ ___|",
    "| |_) | | | |  \\| | |  _ ",
    "|  __/| |_| | |\\  | |_| |",
    "|_|    \\___/|_| \\_|\\____|",
};
inline constexpr int TITLE_ROWS = 5;

} // namespace font
