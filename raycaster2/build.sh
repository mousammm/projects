#!/usr/bin/env bash
set @

mkdir -p bin

WAR="-Wall -Wextra"
BIN="bin/raycaster"
SDL2="`sdl2-config --cflags --libs`"

echo "Compiling and runnning..."
gcc $WAR -o $BIN main.c $SDL2
./$BIN
