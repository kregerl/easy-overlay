cmake_minimum_required(VERSION 3.20)
project(ez_overlay C)

set(CMAKE_C_STANDARD 99)

add_executable(ez_overlay src/main.c src/input.c src/input.h src/renderer.c src/renderer.h)
target_link_libraries(ez_overlay SDL2 SDL2_ttf -lX11 -lXext -lXi)
