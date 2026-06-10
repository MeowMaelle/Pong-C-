# ClaudeTerminalPong — build with `make`, play with `make run`.

CXX      ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic

# Prefer the wide-character ncurses (needed for the UTF-8 block/box glyphs),
# falling back to plain ncurses if ncursesw is unavailable.
NCPKG     := $(shell pkg-config --exists ncursesw && echo ncursesw || echo ncurses)
NC_CFLAGS := $(shell pkg-config --cflags $(NCPKG) 2>/dev/null)
NC_LIBS   := $(shell pkg-config --libs   $(NCPKG) 2>/dev/null || echo -lncurses)

SRCDIR   := src
SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS  := $(SOURCES:.cpp=.o)
DEPS     := $(wildcard $(SRCDIR)/*.h)
TARGET   := pong

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(NC_LIBS)

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) $(NC_CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)
