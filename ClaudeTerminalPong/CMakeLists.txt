cmake_minimum_required(VERSION 3.10)
project(ClaudeTerminalPong CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Prefer the wide-character ncurses for the UTF-8 block/box glyphs.
set(CURSES_NEED_NCURSES TRUE)
set(CURSES_NEED_WIDE TRUE)
find_package(Curses REQUIRED)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(pong ${SOURCES})

target_include_directories(pong PRIVATE ${CURSES_INCLUDE_DIRS})
target_link_libraries(pong PRIVATE ${CURSES_LIBRARIES})

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(pong PRIVATE -Wall -Wextra -Wpedantic)
endif()
